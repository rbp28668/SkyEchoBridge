// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include "state.h"


State::State(){

}

State::~State(){
    for(auto t : traffic){
        delete t.second;
    }
}

void State::receivedTraffic(Target& target){
    std::cout << "Traffic Lat:" << target.latitude << " lon: " << target.longitude << " Alt:" << target.altFeet << "  " << std::string(target.callsign, 8) << std::endl;
    // All Traffic Reports output from the GDL 90 have a Time of Applicability of the beginning of the
    // current second. Therefore, there is no explicit Time of Reception field in the Traffic Report. The
    // Time Stamp conveyed in the most recent Heartbeat message is the Time of Applicability for all
    // Traffic Reports output in that second.

    int identity = target.identity();
    auto it = traffic.find(identity);
    if( it != traffic.end()) {
        TrackedTarget* existing = it->second;
        existing->updateFrom(target);
        existing->markUpdated(uptimeSeconds());
    } else {
        TrackedTarget* newTarget = new TrackedTarget(); // TODO make constructor to take a Target.
        newTarget->updateFrom(target);
        newTarget->markUpdated(uptimeSeconds());
        traffic[identity] = newTarget;
    }


}

void State::receivedOwnship(Target& target){
    if(!target.fixInvalid()) {
        ownship.updateFrom(target);
        ownship.markUpdated(uptimeSeconds());
    }
     
}

void State::setHeightAboveTerrain(int feet){
    heightAboveTerrain = feet;
}

void State::setOwnshipGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning){
    ownship.setGeometricAltitude(altFeet, verticalFigureOfMerit, verticalWarning);
}

void State::setHeartbeat(bool gpsAvailable, bool maintRequired, bool batteryLow, bool utcOk, uint32_t ts, uint32_t receivedMsgCount){
    this->hearbeatTime = ts;
    this->gpsAvailable = gpsAvailable;
    this->maintRequired = maintRequired;
    this->batteryLow = batteryLow;
    this->utcOk = utcOk;
}
