// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "ownship.h"

void OwnShip::markUpdated(long atSeconds){
    lastUpdatedSeconds = atSeconds;
}

void OwnShip::setGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning){
    this->geoAltFeet = altFeet;
    this->geoVerticalFigureOfMerit = verticalFigureOfMerit;
    this->geoVerticalWarning = verticalWarning;
}