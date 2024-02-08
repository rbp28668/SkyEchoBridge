#include "heartbeat_message.h"
#include "clock.h"

HeartbeatMessage::HeartbeatMessage() 
    : gpsAvailable(true)
    , maintRequired(false)
    , batteryLow(false)
    , utcOk(true)
    , ts(0)
    , receivedMsgCount(0)
{

}
size_t HeartbeatMessage::length(){
    return MESSAGE_LENGTH;
}

uint8_t* HeartbeatMessage::data(){
    return message;
}

void HeartbeatMessage::build() {
    message[0] = 0; // message ID

    message[1] = 0x01;
    if(gpsAvailable) message[1] |= 0x80;
    if(maintRequired) message[1] |= 0x40;
    if(batteryLow) message[1] |= 0x08;
    
    message[2] = 0x00;
    if(utcOk) message[2] |= 0x01;

    uint32_t now = Clock::time();
    message[2] |= ((now & 0x10000) ? 0x80 : 0x00);
    message[3] = (now & 0xFF);
    message[4] = (now & 0xFF00) >> 8;
  
    // Ignore UAT received message count
    message[5] = 0;
    message[6] = 0;
}