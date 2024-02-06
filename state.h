// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <list>
#include <unordered_map>

#include <sys/sysinfo.h>

#include "tracked_target.h"
#include "ownship.h"

class OutboundFlarmConverter;

/// @brief Holds the main state of the application
class State {

    // Where to send outbound data
    OutboundFlarmConverter* outbound;

    // This aircraft
    OwnShip ownship;
    
    // Use the Target's identity() function as key.
    std::unordered_map<unsigned int, TrackedTarget*> traffic;


    std::list<TrackedTarget*> activeTraffic;

    // Data from heartbeat.
    uint32_t heartbeatTime;
    bool gpsAvailable;
    bool maintRequired;
    bool batteryLow;
    bool utcOk;
    bool heartbeatReceived;

    int heightAboveTerrain;

    // use system uptime as a proxy for time as clock likely not to be set properly.
    struct sysinfo uptime_info;
    long uptimeSeconds() { ::sysinfo(&uptime_info); return uptime_info.uptime;}
    long lastUptime() { return uptime_info.uptime;}

    void pruneOldTargets();
    static float calculateThreat(float minDist, float verticalSeparation, float atTimeT);

    public:
    State(OutboundFlarmConverter* outbound);
    ~State();
    void receivedTraffic(Target& target);
    void receivedOwnship(Target& target);
    void setHeightAboveTerrain(int feet);
    void setOwnshipGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning);
    void setHeartbeat(bool gpsAvailable, bool maintRequired, bool batteryLow, bool utcOk, uint32_t ts, uint32_t receivedMsgCount);

    void processCurrentState();
};