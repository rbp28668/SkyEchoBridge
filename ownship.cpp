// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "ownship.h"

void OwnShip::markUpdated(long atSeconds){
    lastUpdatedSeconds = atSeconds;
}

void OwnShip::setGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning){
    this->altFeet = altFeet;
    this->verticalFigureOfMerit = verticalFigureOfMerit;
    this->verticalWarning = verticalWarning;
}