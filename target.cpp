// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "target.h"


Target::Target() 
    : trafficAlert(0)
    , addressType(0)
    , address(0)
    , latitude(0)
    , longitude(0)
    , altFeet(0)
    , miscIndicators(0)
    , nic(0)   // Navigation Integrity Category (NIC)
    , ncap(0)   // Navigation Accuracy Category for Position (NACp)
    , emitter(0) // What sort of thing this is - see 3.5.1.10 EMITTER CATEGORY
    , heading(0)
    , speedKts(0xFFF)   // Note 0xFFF or 4095 is unknown.
    , verticalVelocity(0x800) // 0x800 is unknown.
    , track(0)   // see miscIndicators for exactly what this means.
    , callsign({ ' ',' ',' ',' ',' ',' ',' ',' '})
    , emergency(0)

{

}
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
