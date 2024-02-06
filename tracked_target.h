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

    // Threat score of this target.  Range 0 (no threat) to 1 (you've just died)
    float _threat;

    public:
    TrackedTarget();
    int alarm() const { return _alarm; }
    void setAlarm(int level) { _alarm = level;}
    
    float relativeNorth() const { return relativeNorthMetres;}
    float relativeEast() const { return relativeEastMetres;}
    float relativeVertical() const { return relativeVerticalMetres;}
    float relativeBearing() const { return relativeBearingDegrees;}
    float relativeDistance() const { return relativeDistanceMetres;}

    bool advisory() const { return _advisory; }
    void clearAdvisory() { _advisory = _advisorySent = true;}
    void setAdvisory() { _advisory = true; _advisorySent = false;}
    bool advisorySent() { return _advisorySent;}
    void markAdvisorySent() { _advisorySent = true; }

    float threat() const { return _threat;}
    void setThreat(float threat) {_threat =threat;}

    void markUpdated(long atSeconds);
    long lastUpdateTime() const { return lastUpdatedSeconds;}

    std::pair<double, double> extrapolatePosition(uint32_t heartbeatTime) const;
    void setRelativeDistance(float nsMetres, float ewMetres);
    void setRelativeVertical(float metres);
};