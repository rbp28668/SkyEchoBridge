// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "target.h"
#include <cmath>


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
    , speedKts(0xFFF)   // Note 0xFFF or 4095 is unknown.
    , verticalVelocity(0x800) // 0x800 is unknown.
    , track(0)   // see miscIndicators for exactly what this means.
    , callsign{ ' ',' ',' ',' ',' ',' ',' ',' '}
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

/// @brief Return distance in m NS& EW to a target from a reference (typically ownship).
/// Note - assumes the earth is locally flat and has a rectangular coordinate grid.
/// spacing of X is determined by lattitude (i.e. goes to 0 if you're at 90 north).
/// @param target - lat/long of target
/// @param reference - lat/long of reference point
/// @return pair of distances, first is N/S second E/W
std::pair<double,double> Target::distance(const std::pair<double,double>& target, const std::pair<double,double> & reference) {
    //  North/South length per degree lattitude is pretty constant whereas the longitude circle shrinks.
    double metresPerDegreeLat =  M_PI * earthPolarRadius / 180.0;  // polar circumference / 360
    double metresPerDegreeLon =  M_PI * earthEquatorialRadius / 180.0 * std::cos(reference.first * M_PI / 180);

    double deltaNS = (target.first - reference.first) * metresPerDegreeLat;    // +ve if target is east of ref
    double deltaEW = (target.second - reference.second) * metresPerDegreeLon;  // +ve if target is north of ref
 
    return std::pair<double,double>(deltaNS, deltaEW);  // effectively lat/long
}


/// @brief Provides an extrapolated lat/lon deltaSeconds from the last good fix.
/// @param deltaSeconds 
/// @return 
std::pair<double, double> Target::extrapolatePosition(uint32_t deltaSeconds) const{
  
    //  North/South length per degree lattitude is pretty constant whereas the longitude circle shrinks.
    double metresPerDegreeLat =  M_PI * earthPolarRadius / 180.0;  // polar circumference / 360
    double metresPerDegreeLon =  M_PI * earthEquatorialRadius / 180.0 * std::cos(latitude * M_PI / 180);

    double trackRadians = track * M_PI/180;
    double speedMs = ((speedKts == 0xFFF) ? 100 : speedKts) * 0.51444444444;   // 100 kts if unknown - typical light single?

    double deltaEW = speedMs * deltaSeconds * sin(trackRadians);
    double deltaNS = speedMs * deltaSeconds * cos(trackRadians);

    double deltaLat = deltaNS / metresPerDegreeLat;
    double deltaLon = deltaEW / metresPerDegreeLon;

    // Add deltas to lat/lon to give new position
    return std::pair<double,double>(latitude + deltaLat, longitude + deltaLon);
}

/// @brief Gets the target velocity as a pair of numbers in the n/s and e/w direction.
/// @return pair with NS first, EW second NOTE: effectively y,x
std::pair<float, float> Target::velocity() const{
  
    float trackRadians = track * M_PI/180;
    float speedMs = ((speedKts == 0xFFF) ? 100 : speedKts) * 0.51444444444f;   // 100 kts if unknown - typical light single?

    float deltaEW = speedMs * sinf(trackRadians);
    float deltaNS = speedMs * cosf(trackRadians);

    return std::pair<float,float>(deltaNS, deltaEW);
}
