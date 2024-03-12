// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#include <math.h>
#include <string>
#include <ctime>
#include "outbound_flarm_converter.h"
#include "ownship.h"
#include "tracked_target.h"

OutboundFlarmConverter::OutboundFlarmConverter(std::ostream& os)
: os(os)
{

}

// Flarm categories are: 
// 0 = (reserved)
// 1 = glider/motor glider (turbo, self-launch, jet) / TMG
// 2 = tow plane/tug plane
// 3 = helicopter/gyrocopter/rotorcraft
// 4 = skydiver, parachute (Do not use for drop plane!)
// 5 = drop plane for skydivers
// 6 = hang glider (hard)
// 7 = paraglider (soft)
// 8 = aircraft with reciprocating engine(s)
// 9 = aircraft with jet/turboprop engine(s)
// A = unknown
// B = balloon (hot, gas, weather, static)
// C = airship, blimp, zeppelin
// D = unmanned aerial vehicle (UAV, RPAS, drone)
// E = (reserved)
// F = static obstacle
static char typeLookup[] = {
    'A', // 0 - No aircraft type information
    '8', // 1 - Light (ICAO) < 15 500 lbs
    '8', // 2 - Small - 15 500 to 75 000 lbs
    '9', // 3 - Large - 75 000 to 300 000 lbs
    '9', // 4 - High Vortex Large (e.g., aircraft such as B757)
    '9', // 5 - Heavy (ICAO) - > 300 000 lbs
    '9', // 6 - Highly Maneuverable > 5G acceleration and high speed
    '3', // 7 - Rotorcraft
    'A', // 8 - (Unassigned)
    '1', // 9 - Glider/sailplane
    'C', // 10 - Lighter than air
    '4', // 11 - Parachutist/sky diver
    '6', // 12 - Ultra light/hang glider/paraglider
    'A', // 13 - (Unassigned)
    'D', // 14 - Unmanned aerial vehicle
    'A', // 15 - Space/transatmospheric vehicle
    'A', // 16 - (Unassigned)
    'A', // 17 - Surface vehicle — emergency vehicle
    'A', // 18 - Surface vehicle — service vehicle
    'F', // 19 - Point Obstacle (includes tethered balloons)
    'F', // 20 - Cluster Obstacle
    'F', // 21 - Line Obstacle
 
};
char OutboundFlarmConverter::convertAircraftType(uint8_t emitter){
    char result = 'A'; // unknown

    if(emitter < sizeof(typeLookup)) {
        result = typeLookup[emitter];
    }
    return result;
}


void OutboundFlarmConverter::sendTarget(const TrackedTarget& target){
    
    int alarm = target.alarm();
    int relativeNorth = int( floorf(target.relativeNorth() + 0.5f));
    int relativeEast = int( floorf(target.relativeEast() + 0.5f));
    int relativeVertical = int( floorf(target.relativeVertical() + 0.5f));
    bool isICAO = target.addressType == 0 || target.addressType == 2;
    uint32_t id = target.address;
    int track = int(floor(target.track + 0.5f));
    int groundSpeed = target.speedKts * 0.5144444444f;   // kts to m/S
    float climbRate = target.verticalVelocityUnknown() ? 0.0f : target.verticalVelocity * 0.00508f;   // feet per min to m/S
    char acType = convertAircraftType(target.emitter);
    nmea.PFLAA(os, alarm, relativeNorth, relativeEast, relativeVertical, isICAO, id, track, groundSpeed, climbRate, acType);
    os.flush();
}

void OutboundFlarmConverter::sendHeartbeat(int rxCount, bool gpsValid, bool tx, const OwnShip& ownship, TrackedTarget* primaryTarget){

    int gps = gpsValid ? ( ownship.speedKts > 20 ? 2 : 1) : 0;  // 0 invalid, 1 ok - on ground 2 ok - in air. Use speed as proxy for ground.
   

    int alarm = 0;
    int relativeBearing =  NMEAData::EMPTY;
    // 0 = no aircraft within range or no-alarm traffic information
    // 2 = aircraft alarm
    // 4 = traffic advisory (sent once each time an aircraft enters within distance 1.5 km and vertical distance  300 m from own ship;
    int alarmType = 0;
    int relativeVertical = NMEAData::EMPTY;
    int relativeDistance = NMEAData::EMPTY;
    bool isICAO = false;
    uint32_t id = 0;

    if(primaryTarget  ){

        alarm = primaryTarget->alarm();
        relativeBearing = int( floorf(primaryTarget->relativeBearing() + 0.5f));
        //std::cout << "Relative bearing " << relativeBearing << std::endl;
        // Alarm or advisory?  Prioritise alarm!
        if(primaryTarget->alarm() > 0) {
            alarmType = 2;
        } else {
            if(primaryTarget->advisory() && !primaryTarget->advisorySent()) {
                alarm = 1;   // for traffic advisory
                alarmType = 4; // traffic advisory
                primaryTarget->markAdvisorySent();
            }
        }

        relativeVertical = primaryTarget->relativeVertical();
        relativeDistance = primaryTarget->relativeDistance();

        isICAO = primaryTarget->addressType == 0 || primaryTarget->addressType == 2;
        id = primaryTarget->address;

    }
    nmea.PFLAU(os, rxCount, tx, gps, alarm, relativeBearing, alarmType, relativeVertical, relativeDistance, isICAO, id);
    os.flush();
}

void OutboundFlarmConverter::sendOwnshipData(unsigned int utcSeconds, const OwnShip& ownship){
    
    auto position = ownship.extrapolatePosition(utcSeconds);

    double utcTime = utcSeconds;
    double latDegrees = position.first;
    double longDegrees = position.second;
    double groundSpeedKnots = ownship.speedKts;
    double trackDegrees = ownship.track;

    // TODO - whilst this might work it relies on the Pi having the correct time.
    // Given no RTC this may be unlikely.  Would be better to tee off an inbound 
    // GPS feed. OR we take the time from the heartbeat as per spec!
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
     int day = now->tm_mday;
    int month = now->tm_mon +1;
    int year = (now->tm_year) % 100;
    
    double magneticVariationDegrees = 0;
    int altitude = int(floor(ownship.altFeet * (nmea.heightUnits == 'M' ? 0.3048 : 1) + 0.5)); 
    float gpsHeight = ownship.altFeet * (nmea.heightUnits == 'M' ? 0.3048 : 1); 
    //std::cout << "Sending height " << altitude << "," << gpsHeight << nmea.heightUnits << std::endl;
    nmea.GPRMC(os, utcTime, latDegrees, longDegrees, groundSpeedKnots, trackDegrees, day, month, year, magneticVariationDegrees);
    os.flush();
    nmea.PGRMZ(os, altitude);
    os.flush();
    nmea.GPGGA(os, utcTime, latDegrees, longDegrees, gpsHeight);
    os.flush();
}
