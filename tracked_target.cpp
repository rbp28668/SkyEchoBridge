// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "tracked_target.h"
#include <cmath>


TrackedTarget::TrackedTarget()
: _alarm(0)
, _advisory(false)
, _advisorySent(false)
, relativeNorthMetres(0)
, relativeEastMetres(0)
, relativeVerticalMetres(0)
, relativeBearingDegrees(0)
, relativeDistanceMetres(0)
, lastUpdatedSeconds(0)
{

}

void TrackedTarget::markUpdated(long atSeconds){
    lastUpdatedSeconds = atSeconds;
}

std::pair<double, double> TrackedTarget::extrapolatePosition(uint32_t heartbeatTime) const{
    long dt = heartbeatTime - lastUpdatedSeconds;
    return Target::extrapolatePosition(dt);
}

void TrackedTarget::setRelativeDistance(float nsMetres, float ewMetres){
    relativeNorthMetres = nsMetres;
    relativeEastMetres = ewMetres;

    relativeDistanceMetres = fsqrt(nsMetres * nsMetres + ewMetres * ewMetres);

    // Relative bearing degrees - 0 is dead ahead, 90 on the right wingtip etc so range -180 to + 180
    // Swapping y and x rotates by 90 degrees so that this gives the target's bearing relative to the ownship
    relativeBearingDegrees = atan2(ewMetres, nsMetres) * 180 / M_PI;  // TODO check, may need to includ track too - and convert to degrees
    relativeBearingDegrees -= track;
    if(relativeBearingDegrees < -180) {
        relativeBearingDegrees += 360;
    } else if(relativeBearingDegrees > 180){
        relativeBearingDegrees += 360;
    }
}
 
void TrackedTarget::setRelativeVertical(float metres){
    relativeVerticalMetres = metres;
}