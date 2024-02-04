// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "tracked_target.h"

TrackedTarget::TrackedTarget()
: _alarm(0)
, _advisory(false)
, _advisorySent(false)
, relativeNorthMetres(0)
, relativeEastMetres(0)
, relativeVerticalMetres(0)
, relativeBearingDegrees(0)
, relativeDistanceMetres(0)
{

}