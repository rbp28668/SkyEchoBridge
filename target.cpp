// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "target.h"

void Target::updateFrom(const Target& other){
    trafficAlert = other.trafficAlert;
    addressType = other.addressType;
    address = other.address;
    latitude = other.latitude;
    longitude = other.longitude;
    altFeet = other.altFeet;
    miscIndicators = other.miscIndicators;
    nic = other.nic;  
    ncap = other.ncap;   
    emitter = other.emitter; 
    heading = other.heading;
    speedKts = other.speedKts;   
    verticalVelocity = other.verticalVelocity;
    track = other.track;    
    for(int i=0; i< sizeof(callsign); ++i) callsign[i] = other.callsign[i];
    emergency = other.emergency;

}

bool Target::isSameTarget(const Target& other) const {
    return addressType == other.addressType
    && address == other.address
    && emitter == other.emitter
    ;
}


/// @brief sees if the GPS fix in this is valid.
/// If the ownship GPS position fix is invalid, the Latitude, Longitude, and NIC fields in
/// the Ownship Report all have the ZERO value
/// @return 
bool Target::fixInvalid() const{
    return latitude == 0 && longitude == 0 && nic == 0;
}
