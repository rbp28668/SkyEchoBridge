// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include "target.h"

/// @brief A target that's not the ownship -  has relative location and threat.
class TrackedTarget : public Target {
    
    // 0 = no alarm (also used for no-alarm traffic information)
    // 1 = alarm, 13-18 seconds to impact
    // 2 = alarm, 9-12 seconds to impact
    // 3 = alarm, 0-8 seconds to impact
    int _alarm;

    //Flag to send a traffic advisory. Sent once each time an aircraft enters within distance 1.5 km and vertical distance  300 m from own ship
    bool _advisory; 
    bool _advisorySent; // set once set, reset if outside the zone.

    // Where target is relative to ownship.
    float relativeNorthMetres;
    float relativeEastMetres;
    float relativeVerticalMetres;
    float relativeBearingDegrees;
    float relativeDistanceMetres;

    long lastUpdatedSeconds;

    public:
    TrackedTarget();
    int alarm() const { return _alarm; }
    float relativeNorth() const { return relativeNorthMetres;}
    float relativeEast() const { return relativeEastMetres;}
    float relativeVertical() const { return relativeVerticalMetres;}
    float relativeBearing() const { return relativeBearingDegrees;}
    float relativeDistance() const { return relativeDistanceMetres;}

    bool advisory() const { return _advisory; }
    void advisorySent() { _advisorySent = true; }

    void markUpdated(long atSeconds);
};