#include <assert.h>
#include <set>
#include "message_merge.h"
#include "flarm_message.h"

void MessageMerge::PrimaryReceiver::onReceive(const char *data, size_t nbytes){
    FlarmMessage* msg = mm->allocateMessage(data, nbytes);
    mm->receiveFlarm(msg);
}

void MessageMerge::SecondaryReceiver::onReceive(const char *data, size_t nbytes){
    FlarmMessage* msg = mm->allocateMessage(data, nbytes);
    mm->receiveSecondary(msg);
}

MessageMerge::MessageMerge(FlarmMessageWriter* writer)
: writer(writer)
, primaryReceiver(this)
, secondaryReceiver(this)
{
    assert(this);
}

void MessageMerge::setWriter(FlarmMessageWriter* writer){
    assert(this);
    assert(writer != nullptr);
    this->writer = writer;
}


void MessageMerge::receiveFlarm(FlarmMessage* msg){
    if( msg->isValid()) {
        if(msg->isHeartbeat()) {
            primaryHeartbeat = msg;
            sendHeartbeat(); // manage logic to decide which heartbeat to send.
            sendTraffic();   // And traffic 
        } else if(msg->isTraffic()){
            primaryTraffic.push_back(msg);
        } else {
            send(msg);
        }
    }

}

void MessageMerge::receiveSecondary(FlarmMessage* msg){
    assert(this);
    assert(msg);
    if(msg->isValid()){
        if(msg->isHeartbeat()){
            secondaryHeartbeat = msg;
        } else if(msg->isTraffic()) {
            secondaryTraffic.push_back(msg); // will dedupe later.
        }
    }
}

/// @brief manage logic to decide which heartbeat to send.
void MessageMerge::sendHeartbeat(){
    FlarmMessage* toSend = primaryHeartbeat;
    
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

    for(auto t: secondaryTraffic){
        uint32_t addr = t->getId();
        if(addresses.find(addr) != addresses.end()) {
            traffic.push_back(t);
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

void MessageMerge::send(FlarmMessage *msg){
    assert(this);
    assert(msg);
    assert(writer); // must be set up one way or another.
    send_queue.push_back(msg);
    writer->send(this);
}

/// @brief Call to pass raw data from Flarm serial or other source.
/// @param data 
/// @param nbytes 
void MessageMerge::receivePrimaryData(uint8_t* data, size_t nbytes){
    primaryReceiver.receive(data, nbytes);
}

/// @brief Call to pass secondary data from Flarm serial or other source.
/// @param data 
/// @param nbytes 
void MessageMerge::receiveSecondaryData(uint8_t* data, size_t nbytes){
    secondaryReceiver.receive(data, nbytes);
}

/// @brief  Allocates a flarm message and initialises it from the supplied
/// data.  The data should be a correctly framed NMEA message.
/// This therefore creates inbound messages
/// @param data 
/// @param len 
/// @return 
FlarmMessage* MessageMerge::allocateMessage(const char* data, size_t len){
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