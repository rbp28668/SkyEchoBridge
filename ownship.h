// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <utility>
#include "target.h"

/// @brief "Own ship" target to represent this aircraft.
class OwnShip : public Target {
    long lastUpdatedSeconds;

    // If last fix older then treat as invalid position.
    static constexpr int MAX_STALE_FIX = 10; // in seconds

    int geoAltFeet;
    int geoVerticalFigureOfMerit;
    bool geoVerticalWarning;

    public:
    void markUpdated(long atSeconds);
    void setGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning);

    std::pair<double, double> extrapolatePosition(uint32_t heartbeatTime) const;
    bool hasValidPosition(uint32_t heartbeatTime);
};