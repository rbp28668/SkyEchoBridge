// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <list>
#include <unordered_map>

#include <sys/sysinfo.h>

#include "tracked_target.h"
#include "ownship.h"

/// @brief Holds the main state of the application
class State {
    OwnShip ownship;
    
    // Use the Target's identity() function as key.
    std::unordered_map<int, TrackedTarget*> traffic;


    std::list<TrackedTarget*> activeTraffic;

    // Data from heartbeat.
    uint32_t hearbeatTime;
    bool gpsAvailable;
    bool maintRequired;
    bool batteryLow;
    bool utcOk;
    long lastHeartbeatTime;

    int heightAboveTerrain;

    // use system uptime as a proxy for time as clock likely not to be set properly.
    struct sysinfo uptime_info;
    long uptimeSeconds() { ::sysinfo(&uptime_info); return uptime_info.uptime;}
    long lastUptime() { return uptime_info.uptime;}

    public:
    State();
    ~State();
    void receivedTraffic(Target& target);
    void receivedOwnship(Target& target);
    void setHeightAboveTerrain(int feet);
    void setOwnshipGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning);
    void setHeartbeat(bool gpsAvailable, bool maintRequired, bool batteryLow, bool utcOk, uint32_t ts, uint32_t receivedMsgCount);
};