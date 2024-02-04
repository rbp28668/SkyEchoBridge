// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once
#include <cstdint>

/// @brief Information received from a GDL-90 message for ownship or target.
class Target {

    public:

    // Variables received from the message
    bool trafficAlert;
    uint8_t addressType;
    uint32_t address;
    double latitude;
    double longitude;
    int altFeet;
    uint8_t miscIndicators;
    uint8_t nic;    // Navigation Integrity Category (NIC)
    uint8_t ncap;   // Navigation Accuracy Category for Position (NACp)
    uint8_t emitter; // What sort of thing this is - see 3.5.1.10 EMITTER CATEGORY
    float heading;
    int speedKts;   // Note 0xFFF or 4095 is unknown.
    int verticalVelocity; // 0x800 is unknown.
    float track;    // see miscIndicators for exactly what this means.
    char callsign[8];
    uint8_t emergency;

    
    
    public:
    unsigned int identity() { return uint32_t(addressType) << 24 | address;}
    void updateFrom(const Target& other);
    bool isSameTarget(const Target& other) const;
    bool fixInvalid() const;
    bool speedUnknown() const {return speedKts == 0xFFF;}
    bool verticalVelocityUnknown() const {return verticalVelocity == 0x800;}

    Target();
};