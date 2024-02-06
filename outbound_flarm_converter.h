// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once
#include <cstdint>
#include <iostream>
#include "NMEAData.h"

class TrackedTarget;
class OwnShip;

/// @brief Converts GDL-90 data to Flarm/NMEA format
/// @note  not static as we're likely to want to configure it at
/// some point to throttle and/or reduce output for merging
/// into flarm data.
class OutboundFlarmConverter {
    
    std::ostream& os;
    NMEAData nmea;
    char convertAircraftType(uint8_t emitter);

public:
    OutboundFlarmConverter(std::ostream& os);
    void sendTarget(const TrackedTarget& target);
    void sendHeartbeat(int rxCount, bool gpsValid, const OwnShip& ownship, TrackedTarget* primaryTarget);
    void sendOwnshipData(unsigned int utcSeconds, const OwnShip& ownship);
};