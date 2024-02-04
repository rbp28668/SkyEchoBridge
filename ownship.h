// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include "target.h"

/// @brief "Own ship" target to represent this aircraft.
class OwnShip : public Target {
    long lastUpdatedSeconds;


    int altFeet;
    int verticalFigureOfMerit;
    bool verticalWarning;

    public:
    void markUpdated(long atSeconds);
    void setGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning);
};