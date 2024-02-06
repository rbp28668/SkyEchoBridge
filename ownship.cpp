// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#include <cmath>
#include "ownship.h"

void OwnShip::markUpdated(long atSeconds){
    lastUpdatedSeconds = atSeconds;
}

void OwnShip::setGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning){
    this->geoAltFeet = altFeet;
    this->geoVerticalFigureOfMerit = verticalFigureOfMerit;
    this->geoVerticalWarning = verticalWarning;
}

std::pair<double, double> OwnShip::extrapolatePosition(uint32_t heartbeatTime) const{
    long dt = heartbeatTime - lastUpdatedSeconds;
    return Target::extrapolatePosition(dt);
}


bool OwnShip::hasValidPosition(uint32_t heartbeatTime){
    if(fixInvalid()) return false;

    return (heartbeatTime - lastUpdatedSeconds) < MAX_STALE_FIX;
}