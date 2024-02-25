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

MessageMerge::MessageMerge()
: primaryReceiver(this)
, secondaryReceiver(this)
{

}

void MessageMerge::receiveFlarm(FlarmMessage* msg){
    if( msg->isValid()) {
        if(msg->isFlarm()) {
            // Process FLARM specific messages.

        } else {
            send(msg);
        }
    }

}

void MessageMerge::receiveSecondary(FlarmMessage* msg){
    if(msg->isValid()){
        if(msg->isHeartbeat()){
            secondaryHeartbeat = msg;
        } else if(msg->isTraffic()) {
            traffic.push_back(msg); // will dedupe later.
        }
    }
}

void MessageMerge::send(FlarmMessage *msg){
    send_queue.push_back(msg);
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
/// @param data 
/// @param len 
/// @return 
FlarmMessage* MessageMerge::allocateMessage(const char* data, size_t len){
    FlarmMessage* msg = new FlarmMessage(data, len);
}
