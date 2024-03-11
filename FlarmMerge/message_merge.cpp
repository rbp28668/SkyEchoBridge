#include <assert.h>
#include <set>
#include <iostream>
#include "message_merge.h"
#include "flarm_message.h"

/// @brief Receives raw flarm data, converts it to a FlarmMessage and
/// passes it to the MessageMerge as a primary message.
/// @param data 
/// @param nbytes 
void MessageMerge::PrimaryReceiver::onReceive(const char *data, size_t nbytes){
    assert(this);
    assert(data);
    assert(nbytes > 0);

    //std::cout << std::string(data, nbytes);  // should have crlf
    FlarmMessage* msg = mm->allocateMessage(data, nbytes);
    mm->receiveFlarm(msg);
}

/// @brief Receives raw flarm data, converts it to a FlarmMessage and
/// passes it to the MessageMerge as a secondary (ADSB derived) message.
/// @param data 
/// @param nbytes 
void MessageMerge::SecondaryReceiver::onReceive(const char *data, size_t nbytes){
    assert(this);
    assert(data);
    assert(nbytes > 0);
    
    FlarmMessage* msg = mm->allocateMessage(data, nbytes);
    mm->receiveSecondary(msg);
}

/// @brief Creates a message merger. 
/// Rule for message handling - delete it or send it.
/// @param writer to receive outbound messages, maybe null
/// in which case setWriter should be called.
MessageMerge::MessageMerge(FlarmMessageWriter* writer)
: writer(writer)
, primaryReceiver(this)
, secondaryReceiver(this)
{
    assert(this);
}

/// @brief Sets the writer for outbound messages.  Decouples
/// setting the writer from creation of the merger.
/// @param writer to receive messages, not null.
void MessageMerge::setWriter(FlarmMessageWriter* writer){
    assert(this);
    assert(writer != nullptr);
    this->writer = writer;
}

/// @brief Receives a message from the primary source.  Flarm
/// heartbeat messages trigger sending of the heartbeat - sent
/// immediately so this doesn't add significant delay to critical 
/// alerts, followed by traffic data.
/// Non-flarm (i.e. standard GPS) messages are passed through.
/// @param msg 
void MessageMerge::receiveFlarm(FlarmMessage* msg){
    assert(this);
    assert(msg);
    assert(msg->length() > 0);

    if( msg->isValid()) {
        if(msg->isHeartbeat()) {
            secondaryCount = 0; 
            if(primaryHeartbeat != nullptr) delete primaryHeartbeat;
            primaryHeartbeat = msg;
            hasPrimaryFix = primaryHeartbeat->hasGps();

            sendHeartbeat(); // manage logic to decide which heartbeat to send.
            sendTraffic();   // And traffic 
        } else if(msg->isTraffic()){
            primaryTraffic.push_back(msg);
        } else {
            // Probably just a normal GPS message.
            send(msg);
        }
    } else {
        delete msg; // not valid so discard.
    }

}

// Receives a message from the secondary source.  Generally they're just
// queued but if there have beeen a few secondary heatbeats without 
// anything from the primary this starts to trigger sending.
void MessageMerge::receiveSecondary(FlarmMessage* msg){
    assert(this);
    assert(msg);
    assert(msg->length() > 0);
    if(msg->isValid()){
        if(msg->isHeartbeat()){
            if(secondaryHeartbeat != nullptr) delete secondaryHeartbeat;
            secondaryHeartbeat = msg;
            ++secondaryCount; // will be reset by receiving primary.

            // Only send here if we're not getting primary messages. Otherwise
            // actual sending will be triggered by receiving heartbeat from primary.
            if(secondaryActive()){
                sendHeartbeat(); // manage logic to decide which heartbeat to send.
                sendTraffic();   // And traffic 
            }
        } else if(msg->isTraffic()) {
            secondaryTraffic.push_back(msg); // will dedupe later.
        } else { // Not a flarm specific message
            // If primary has dropped out or has no gps fix then we want to send it otherwise just drop.
            if(secondaryActive() || !hasPrimaryFix){
                send(msg);
            } else {
                // Not one we're interested in
                delete msg;
            }
         }
    } else {
        delete msg; // not valid so discard.
    }
}

/// @brief manage logic to decide which heartbeat to send.
/// postcondition: both heartbeatMessages are null.
void MessageMerge::sendHeartbeat(){
    FlarmMessage* toSend = primaryHeartbeat;
    
    if(primaryHeartbeat == nullptr && secondaryHeartbeat == nullptr){
        return;
    }
    
    if(primaryHeartbeat == nullptr) {
        assert(secondaryHeartbeat != nullptr);
        send(secondaryHeartbeat);
        secondaryHeartbeat = nullptr;
    } else if(secondaryHeartbeat == nullptr){
        assert(primaryHeartbeat != nullptr);
        send(primaryHeartbeat);
        primaryHeartbeat = nullptr;
    } else {

        // Both primary and secondary have data
        if(secondaryHeartbeat->hasGps() && !primaryHeartbeat->hasGps()){
            toSend = secondaryHeartbeat;
        }

        if(secondaryHeartbeat->hasAdvisory()  && !primaryHeartbeat->hasAdvisory() ){
            toSend = secondaryHeartbeat;
        }

        if(primaryHeartbeat->alarmLevel() > 0 || secondaryHeartbeat->alarmLevel() > 0){
            toSend = (primaryHeartbeat->alarmLevel() >= secondaryHeartbeat->alarmLevel()) ? primaryHeartbeat : secondaryHeartbeat;
        }

        // The message that isn't sent should be deleted.
        FlarmMessage* toDelete = (toSend == primaryHeartbeat) ? secondaryHeartbeat : primaryHeartbeat;

        send(toSend);
        delete toDelete;
        primaryHeartbeat = nullptr;
        secondaryHeartbeat = nullptr;
    }

    assert(primaryHeartbeat == nullptr);
    assert(secondaryHeartbeat == nullptr);
 } 

/// @brief Send traffic messages, deduplicating / sorting
/// as needed.
void MessageMerge::sendTraffic(){
    
    // Track Ids to remove any ADSB that already has a FLARM record
    std::set<uint32_t> addresses;
    
    std::vector<FlarmMessage*> traffic;
    traffic.reserve( primaryTraffic.size() + secondaryTraffic.size());

    for( auto t : primaryTraffic){
        uint32_t addr = t->getId();
        if(addr != 0) addresses.insert(addr);
        traffic.push_back(t);
    }

    
    // May have duplicates in secondaries if primary timing doesn't align so 
    // dedupe these as well. End of queue is most recent so start from here
    // and run backwards.
    for (auto it = secondaryTraffic.rbegin(); it != secondaryTraffic.rend(); ++it){
        FlarmMessage* t = *it;
        uint32_t addr = t->getId();
        if(addresses.find(addr) != addresses.end()) {
            traffic.push_back(t);
            addresses.insert(addr); //
        } else {   // it's a duplicate target so discard.
            delete t;
        }
    }

    primaryTraffic.clear();
    secondaryTraffic.clear();

    // Ideally sort the list into ascending order of threat then can discard
    // lowest.  For the time being at the moment, send everything.
    // Note 38400 baud -> 3840 bytes.  Assume 80 bytes per message that's 48 messages
    // Probably don't want more than 40 to allow for the PFLAU, and GPS messages.
    // At the moment, leave alone.
    for(auto t : traffic){
        send(t);
    }
}

/// @brief Pushes a message on the send queue and invokes the writer
/// to start sending.  Any sent message should result in a call
/// back to messageConsumed.
/// @param msg 
void MessageMerge::send(FlarmMessage *msg){
    assert(this);
    assert(msg);
    assert(writer); // must be set up one way or another.
    assert(msg->length() > 0);
    send_queue.push_back(msg);
    writer->send(this);
}

/// @brief Call to pass raw data from Flarm serial or other source.
/// @param data 
/// @param nbytes 
void MessageMerge::receivePrimaryData(uint8_t* data, size_t nbytes){
    assert(this);
    assert(data);
    assert(nbytes > 0);
    primaryReceiver.receive(data, nbytes);
}

/// @brief Call to pass secondary data from Flarm serial or other source.
/// @param data 
/// @param nbytes 
void MessageMerge::receiveSecondaryData(uint8_t* data, size_t nbytes){
    assert(this);
    assert(data);
    assert(nbytes > 0);
    secondaryReceiver.receive(data, nbytes);
}

/// @brief  Allocates a flarm message and initialises it from the supplied
/// data.  The data should be a correctly framed NMEA message.
/// This therefore creates inbound messages
/// @param data 
/// @param len 
/// @return 
FlarmMessage* MessageMerge::allocateMessage(const char* data, size_t len){
    assert(this);
    assert(data);
    assert(len > 0);
    FlarmMessage* msg = new FlarmMessage(data, len);
    return msg;
}

/// @brief Gets any outbound Flarm message that needs to be sent.
/// Once sent messageConsumed(FlarmMessage*) should be called to remove
/// the message from the queue.
/// @return Next message to send or nullptr if none;
FlarmMessage* MessageMerge::getOutboundMessage(){
    if(send_queue.empty()){
        return nullptr;
    } else {
        return send_queue.front();
    }
}

/// @brief Call this once a Flarm message has been sent so that it
/// can be removed from the send queue and deleted.
/// @param msg 
void MessageMerge::messageConsumed(FlarmMessage* msg){
    assert(this);
    assert(msg);
    assert(!send_queue.empty());
    
    FlarmMessage* front = send_queue.front();
    assert(msg == front);

    send_queue.pop_front();
    delete front;
}