// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include "state.h"


State::State(){

}

State::~State(){
    for(auto t : traffic){
        delete t;
    }
}

void State::receivedTraffic(Target& target){
    std::cout << "Traffic Lat:" << target.latitude << " lon: " << target.longitude << " Alt:" << target.altFeet << "  " << std::string(target.callsign, 8) << std::endl;
    // All Traffic Reports output from the GDL 90 have a Time of Applicability of the beginning of the
    // current second. Therefore, there is no explicit Time of Reception field in the Traffic Report. The
    // Time Stamp conveyed in the most recent Heartbeat message is the Time of Applicability for all
    // Traffic Reports output in that second.
}

void State::receivedOwnship(Target& target){
    
}

void State::setHeightAboveTerrain(int feet){

}

void State::setOwnshipGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning){

}

void State::setHeartbeat(bool gpsAvailable, bool maintRequired, bool batteryLow, bool utcOk, uint32_t ts, uint32_t receivedMsgCount){
    this->hearbeatTime = ts;
    this->gpsAvailable = gpsAvailable;
    this->maintRequired = maintRequired;
    this->batteryLow = batteryLow;
    this->utcOk = utcOk;
}
