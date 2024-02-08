// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include "message.h"

/// @brief Information received from a GDL-90 message for ownship or target.
class TargetMessage : public Message{

 
    static constexpr int MESSAGE_LENGTH = 28;
    uint8_t message[MESSAGE_LENGTH];

    public:

    bool isOwnship;
    // Values to stuff into the message
    bool trafficAlert;
    uint8_t addressType;
    uint32_t address;
    double latitude;
    double longitude;
    int altFeet;
    uint8_t miscIndicators;
    uint8_t nic;    // Navigation Integrity Category (NIC)
    uint8_t nacp;   // Navigation Accuracy Category for Position (NACp)
    uint8_t emitter; // What sort of thing this is - see 3.5.1.10 EMITTER CATEGORY
    int speedKts;   // Note 0xFFF or 4095 is unknown.
    int verticalVelocity; // 0x800 is unknown.
    float track;    // see miscIndicators for exactly what this means.
    char callsign[8];
    uint8_t emergency;

    
    protected:
    
    static constexpr double earthEquatorialRadius = 6378.1 * 1000.0; //  metres
    static constexpr double earthPolarRadius = 6356.8 * 1000.0; // metres

    public:
    TargetMessage();

    virtual size_t length();
    virtual uint8_t* data();

    void setCallsign(const std::string& cs);
    void build();
};

class Ownship : public TargetMessage
{
    public: 
    Ownship() {isOwnship = true;}
};

class Traffic : public TargetMessage
{
    public: 
    Traffic() {isOwnship = false;}
};

