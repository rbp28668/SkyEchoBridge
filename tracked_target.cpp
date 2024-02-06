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
    relativeBearingDegrees = atan2(nsMetres, ewMetres);  // TODO check
}
 
void TrackedTarget::setRelativeVertical(float metres){
    relativeVerticalMetres = metres;
}