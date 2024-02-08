// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#include <iostream>
#include <assert.h>
#include <math.h>
#include "message_handler.h"
#include "fcs.h"
#include "target.h"
#include "state.h"

MessageHandler::MessageHandler(State* state)
: state(state)
{

}

/// @brief Sees if the message has a valid frame check sequence / CRC
/// @param msg 
/// @param len 
/// @return 
bool MessageHandler::isFcsValid(uint8_t* msg, size_t len){
    if(len < 2) return false;

    // FCS should be last 2 bytes of the message, least significant byte first
    // See 2.2.1. Datalink Structure and Processing
    uint16_t msgFcs = msg[len-1];
    msgFcs <<= 8;
    msgFcs |= msg[len-2];

    uint16_t calcFcs = FCS::compute(msg, len-2);
    return  msgFcs == calcFcs;
}

/// @brief Processes a message containing target info whether traffic or ownship
/// @param target 
/// @param msgStart 
/// @param msgLength 
void MessageHandler::processTargetMessage(Target* target, uint8_t* msg, size_t msgLength){

    //Traffic Report data = st aa aa aa ll ll ll nn nn nn dd dm ia hh hv vv tt ee cc cc cc cc cc cc cc cc px
    
    // Traffic Alert Status. s = 1 indicates that a Traffic Alert is active for this target.
    target->trafficAlert =  ((msg[1] & 0xF0) >> 4) == 1;

    // Address Type: Describes the type of address conveyed in the Participant Address field
    target->addressType = msg[1] & 0x0F;

    //Participant Address (24 bits).
    target->address = (uint32_t(msg[2]) << 16) | (uint32_t(msg[3]) << 8) | uint32_t(msg[4]);
    
    //Latitude: 24-bit signed binary fraction. Resolution = 180 / 2^23 degrees.
    uint32_t ulat = (uint32_t(msg[5]) << 16) | (uint32_t(msg[6]) << 8) | uint32_t(msg[7]);
    ulat |= (ulat & 0x00800000) != 0 ? 0xFF000000 : 0; // sign extend to 32 bits
    target->latitude = int32_t(ulat) * 180.0 / 8388608;

    //Longitude: 24-bit signed binary fraction. Resolution = 180 / 2^23 degrees.
    uint32_t ulon = (uint32_t(msg[8]) << 16) | (uint32_t(msg[9]) << 8) | uint32_t(msg[10]);
    ulon |= (ulon & 0x00800000) != 0 ? 0xFF000000 : 0; // sign extend to 32 bits
    target->longitude = int32_t(ulon) * 180.0 / 8388608;

    //Altitude: 12-bit offset integer. Resolution = 25 feet. Altitude (ft) = ("ddd" * 25) - 1,000
    target->altFeet = int((uint32_t(msg[11]) << 4) | ((msg[12] & 0xF0) >>4)) * 25 - 1000;

    //Miscellaneous indicators: (see text)
    target->miscIndicators = msg[12] & 0x0F;

    // Navigation Integrity Category (NIC):
    target->nic = (msg[13] & 0xF0) >> 4;

    // Navigation Accuracy Category for Position (NACp):
    target->ncap = (msg[13] & 0x0F);

    /// Horizontal velocity. Resolution = 1 kt.
    target->speedKts = int(uint32_t(msg[14]) << 4 | ((msg[15] & 0xF0) >> 4));

    /// vvvVertical Velocity: Signed integer in units of 64 fpm.
    target->verticalVelocity = int( (uint32_t(msg[15] & 0x0F) << 8 ) | uint32_t(msg[16])) * 64;

    // ttTrack/Heading: 8-bit angular weighted binary. Resolution = 360/256 degrees.
    // 0 = North, 128 = South. See Miscellaneous field for Track/Heading indication.
    target->track = int(floor(msg[17] * 360.0f / 256.0f + 0.5f));
     
    // eeEmitter Category
    target->emitter = msg[18];

    // Call Sign: 8 ASCII characters, '0' through '9' and 'A' through 'Z'.
    target->callsign[0] = msg[19];
    target->callsign[1] = msg[20];
    target->callsign[2] = msg[21];
    target->callsign[3] = msg[22];
    target->callsign[4] = msg[23];
    target->callsign[5] = msg[24];
    target->callsign[6] = msg[25];
    target->callsign[7] = msg[26];

    // pEmergency/Priority Code:
    target->emergency = (msg[27] & 0xF0) >> 4;

    // xSpare (reserved for future use)
}

void MessageHandler::handleHeartbeat(uint8_t* msg, size_t msgLength){
    assert(*msg == 0);
    assert(msgLength == 7); 
    if(msgLength != 7) return;

    bool gpsAvailable = (msg[1] & 0x80) != 0;
    bool maintRequired = (msg[1] & 0x40) != 0;
    bool batteryLow = (msg[1] & 0x04) != 0;
    bool utcOk = (msg[2] & 0x01) != 0;
    uint32_t ts = (uint32_t(msg[4]) << 8) | uint32_t(msg[3]); // spec says LS byte first
    ts |= ((msg[2] & 0x80) != 0) ? 0x00010000 : 0;  // and fold in bit 17
    uint32_t receivedMsgCount = ( uint32_t(msg[5]) << 8) | uint32_t(msg[6]);

    state->setHeartbeat(gpsAvailable, maintRequired, batteryLow, utcOk, ts, receivedMsgCount);
}

void MessageHandler::handleUplinkData(uint8_t* msgStart, size_t msgLength){
    // NOP
}
void MessageHandler::handleHeightAboveTerrain(uint8_t* msg, size_t msgLength){
    assert(*msg == 9);
    assert(msgLength == 3); 
    if(msgLength != 3) return;

    int hat = (uint32_t(msg[1]) << 8) | uint32_t(msg[2]);
    state->setHeightAboveTerrain(hat);
}

void MessageHandler::handleOwnshipReport(uint8_t* msgStart, size_t msgLength){
    assert(*msgStart == 10);
    assert(msgLength == 28); 
    if(msgLength != 28) return;
    Target target;
    processTargetMessage(&target, msgStart, msgLength);
    state->receivedOwnship(target);
}

void MessageHandler::handleOwnshipGeometricAltitude(uint8_t* msg, size_t msgLength){
    assert(*msg == 11);
    assert(msgLength == 5); 
    if(msgLength != 5) return;

    int alt = int((uint32_t(msg[1]) << 8) || uint32_t(msg[2])) * 5;

    uint32_t vm = (uint32_t(msg[3]) << 8) || uint32_t(msg[4]);

    int verticalFigureOfMerit = int(vm & 0x7FFF);
    bool verticalWarning = (vm & 0x8000) != 0;
    state->setOwnshipGeometricAltitude(alt, verticalFigureOfMerit, verticalWarning);
}

void MessageHandler::handleTrafficReport(uint8_t* msgStart, size_t msgLength){
    assert(*msgStart == 20);
    assert(msgLength == 28); 
    if(msgLength != 28) return;

    Target target;
    processTargetMessage(&target, msgStart, msgLength);
    state->receivedTraffic(target);
}

void MessageHandler::handleBasicReport(uint8_t* msgStart, size_t msgLength){
    // NOP
}
void MessageHandler::handleLongReport(uint8_t* msgStart, size_t msgLength){
    // NOP
}
void MessageHandler::handleForeFlightMessage(uint8_t* msgStart, size_t msgLength){
    // NOP
}

void MessageHandler::onMessage(uint8_t* msg, size_t len){

    if(len < 2) {
        std::cout << "Runt message " << len << " bytes" << std::endl;
        return;
    } 

    if(isFcsValid(msg, len)){

        len -= 2; // ignore FCS now

        uint8_t messageId = *msg;

        // In equipment that complies with this version of this interface document, the most significant bit
        // of the Message ID will always be ZERO, so that the Message ID can be treated as an 8-bit value
        // with a range of 0-127 (decimal). Any messages that have a Message ID outside of this range
        // should be discarded.
        if(messageId > 127) return;

        switch(static_cast<MessageType>(messageId)){
            case MessageType::Heartbeat:
                assert(len == 7);
                handleHeartbeat(msg, len);
                break;

            case MessageType::UplinkData:
                std::cout << "UplinkData" << std::endl;
                assert(len == 436);
                handleUplinkData(msg, len);
                break;

            case MessageType::HeightAboveTerrain:
                std::cout << "HAT" << std::endl;
                assert(len == 3);
                handleHeightAboveTerrain(msg, len);
                break;

            case MessageType::OwnshipReport:
                assert(len == 28);
                handleOwnshipReport(msg, len);
                break;

            case MessageType::OwnshipGeometricAltitude:
                std::cout << "Ownship Geometric Altitude" << std::endl;
                handleOwnshipGeometricAltitude(msg, len);
                break;

            case MessageType::TrafficReport:
                assert(len == 28);
                handleTrafficReport(msg, len);
                break;

            case MessageType::BasicReport:
                std::cout << "Basic Report" << std::endl;
                assert(len == 22);
                handleBasicReport(msg, len);
                break;

            case MessageType::LongReport:
                std::cout << "Long Report" << std::endl;
                assert(len == 38);
                handleLongReport(msg,len);
                break;

            case MessageType::IdMessage:
                // std::cout << "ID Message" << std::endl;
                // Technically a ForeFlight message - see https://www.foreflight.com/connect/spec/
                // Byte 2 determines sub message type
                break;

            default:
                std::cout << "Unknown message type " << int(messageId) << std::endl;
                break;
        }
    } else {
        std::cout << "FCS mismatch" << std::endl;
    }
  

}