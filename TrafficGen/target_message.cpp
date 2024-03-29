// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "target_message.h"
#include <iostream>
#include <cmath>

TargetMessage::TargetMessage()
    : trafficAlert(0), addressType(0), address(0), latitude(0), longitude(0), altFeet(0), miscIndicators(0), nic(0) // Navigation Integrity Category (NIC)
      ,
      nacp(0) // Navigation Accuracy Category for Position (NACp)
      ,
      emitter(0) // What sort of thing this is - see 3.5.1.10 EMITTER CATEGORY
      ,
      speedKts(0xFFF) // Note 0xFFF or 4095 is unknown.
      ,
      verticalVelocity(0x800) // 0x800 is unknown.
      ,
      track(0) // see miscIndicators for exactly what this means.
      ,
      callsign{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, emergency(0), descentFPM(0) // would be nice!

{
}

size_t TargetMessage::length()
{
    return MESSAGE_LENGTH;
}

uint8_t *TargetMessage::data()
{
    return message;
}

void TargetMessage::setCallsign(const std::string &cs)
{
    for (int i = 0; i < 8; ++i)
    {
        if (i >= cs.length())
            break;
        callsign[i] = cs.at(i);
    }
}

void TargetMessage::build()
{
    message[0] = isOwnship ? 10 : 20;
    message[1] = trafficAlert ? 0x10 : 0x00;
    message[1] |= (addressType & 0x0F);

    message[2] = (address & 0xFF0000) >> 16;
    message[3] = (address & 0x00FF00) >> 8;
    message[4] = (address & 0x0000FF);

    int lat = latitude * 8388608.0 / 180.0;
    message[5] = (lat & 0xFF0000) >> 16;
    message[6] = (lat & 0x00FF00) >> 8;
    message[7] = (lat & 0x0000FF);

    int lon = longitude * 8388608.0 / 180.0;
    message[8] = (lon & 0xFF0000) >> 16;
    message[9] = (lon & 0x00FF00) >> 8;
    message[10] = (lon & 0x0000FF);

    int alt = int(floor((altFeet + 1000) / 25 + 0.5));
    message[11] = (alt & 0x0FF0) >> 4;
    message[12] = (alt & 0x000F) << 4;

    message[12] |= (miscIndicators & 0x0F);

    message[13] = ((nic & 0x0F) << 4) | (nacp & 0x0F);

    int kts = speedKts;
    if (kts >= 4094)
        kts = 0xFFE;
    message[14] = (kts & 0x0FF0) >> 4;
    message[15] = (kts & 0x000F) << 4;

    int vv = verticalVelocity / 64;
    message[15] |= (vv & 0xF00) >> 8;
    message[16] = vv & 0x00FF;

    int trk = int(floor(0.5 + track * 256.0f / 360.0f));
    message[17] = trk & 0xFF;

    message[18] = emitter;

    message[19] = callsign[0];
    message[20] = callsign[1];
    message[21] = callsign[2];
    message[22] = callsign[3];
    message[23] = callsign[4];
    message[24] = callsign[5];
    message[25] = callsign[6];
    message[26] = callsign[7];

    message[27] = (emergency & 0x0F) << 4;
}

void TargetMessage::move(double north, double east)
{
    double metresPerDegreeLat = M_PI * earthPolarRadius / 180.0; // polar circumference / 360
    double metresPerDegreeLon = M_PI * earthEquatorialRadius / 180.0 * std::cos(latitude * M_PI / 180);

    double deltaLat = north / metresPerDegreeLat;
    double deltaLon = east / metresPerDegreeLon;

    latitude += deltaLat;
    longitude += deltaLon;
}
void TargetMessage::tick()
{
    //  North/South length per degree lattitude is pretty constant whereas the longitude circle shrinks.
    double metresPerDegreeLat = M_PI * earthPolarRadius / 180.0; // polar circumference / 360
    double metresPerDegreeLon = M_PI * earthEquatorialRadius / 180.0 * std::cos(latitude * M_PI / 180);

    double trackRadians = track * M_PI / 180;
    double speedMs = speedKts * 0.51444444444;

    int deltaSeconds = 1; // assume called once a second.
    double deltaEW = speedMs * deltaSeconds * sin(trackRadians);
    double deltaNS = speedMs * deltaSeconds * cos(trackRadians);

    double deltaLat = deltaNS / metresPerDegreeLat;
    double deltaLon = deltaEW / metresPerDegreeLon;

    latitude += deltaLat;
    longitude += deltaLon;

    altFeet -= descentFPM / 60.0;
}

/// @brief Moves this aircraft to be on a conflicting course with the other aircraft.
/// Changes the lat/lon & track of this target to bring it into conflict.
/// @param other is the aircraft to conflict with- usually ownship
/// @param targetRangeNM how far away to start the approach
/// @param relativeBearing where should this appear relative to the other aircraft
void TargetMessage::makeConflictingTo(TargetMessage* other, double targetRangeNM, double relativeBearing){
    double R = 6371 / 1.852; // radius of earth in NM (convert km to NM)
	double pi = 3.1415926535897932384636434;

	// How long to reach me?
	double t = targetRangeNM / this->speedKts;

	// Use these to scale x,y back to lat/long.  Assumes a rectangular grid centred on
	// the current position.  Only valid if distances are small.
	double distPerDegreeLong = 2 * pi * R * cos(other->latitude * pi / 180) / 360;
	double distPerDegreeLat = 2 * pi * R / 360;

	// Work out how far we've moved before target gets to our location (over time t).
	double h = other->track * pi / 180; // my heading in radians
	double dx = t * other->speedKts * sin(h);
	double dy = t * other->speedKts * cos(h);

	// From that position work out where target should start
	double th = (other->track + relativeBearing) * pi / 180;  // reciprocal target heading - go this way to see where target should start;
	dx += targetRangeNM * sin(th);
	dy += targetRangeNM * cos(th);

	// Convert position offsets to nominal lat/long assuming rectangular grid;
	double tLong = other->longitude + dx / distPerDegreeLong;
	double tLat = other->latitude + dy / distPerDegreeLat;

    this->latitude = tLat;
    this->longitude =tLong;
    this->track = other->track + relativeBearing + 180; // 180 as coming from, not going to
    while (this->track >= 360) this->track -= 360;
	
  
}

double TargetMessage::distanceTo(TargetMessage* other){
        //  North/South length per degree lattitude is pretty constant whereas the longitude circle shrinks.
    double metresPerDegreeLat =  M_PI * earthPolarRadius / 180.0;  // polar circumference / 360
    double metresPerDegreeLon =  M_PI * earthEquatorialRadius / 180.0 * std::cos(this->latitude * M_PI / 180);

    double deltaNS = (other->latitude - this->latitude) * metresPerDegreeLat;    // +ve if target is east of ref
    double deltaEW = (other->longitude - other->longitude) * metresPerDegreeLon;  // +ve if target is north of ref

    double dist = sqrt(deltaNS * deltaNS + deltaEW * deltaEW);
    return dist;
}
