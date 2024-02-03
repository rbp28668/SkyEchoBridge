#include "target.h"

bool Target::isSameTarget(Target& other){
    return addressType == other.addressType
    && address == other.address
    && emitter == other.emitter
    ;
}


/// @brief sees if the GPS fix in this is valid.
/// If the ownship GPS position fix is invalid, the Latitude, Longitude, and NIC fields in
/// the Ownship Report all have the ZERO value
/// @return 
bool Target::fixInvalid(){
    return latitude == 0 && longitude == 0 && nic == 0;
}
