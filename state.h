// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <list>

#include "target.h"
/// @brief Holds the main state of the application
class State {
    Target ownship;
    std::list<Target*> traffic;

    // Data from heartbeat.
    uint32_t hearbeatTime;
    bool gpsAvailable;
    bool maintRequired;
    bool batteryLow;
    bool utcOk;


    public:
    State();
    ~State();
    void receivedTraffic(Target& target);
    void receivedOwnship(Target& target);
    void setHeightAboveTerrain(int feet);
    void setOwnshipGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning);
    void setHeartbeat(bool gpsAvailable, bool maintRequired, bool batteryLow, bool utcOk, uint32_t ts, uint32_t receivedMsgCount);
};