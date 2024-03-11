// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "NMEAData.h"


// Converts lat to appropriate degrees and N/S
std::ostream& NMEAData::convertLat(std::ostream& os, double latDegrees) const {
	
	int degrees, minutes, fraction;

	bool isNorth = latDegrees >= 0.0;
	double absLat = std::abs(latDegrees);

	// Lattitude, first 2 digits are whole degrees.
	degrees = (int)std::floor(absLat);
	absLat -= degrees;  // now fractional degrees.
	absLat *= 60;			// now minutes of arc.
	minutes = (int)std::floor(absLat);
	absLat -= minutes;		// decimal fraction of minute of arc.
	fraction = (int)std::floor(absLat * 10000);
	os << std::setw(2) << std::setfill('0') << degrees;
	os << std::setw(2) << std::setfill('0') << minutes;
	os << '.';
	os << std::setw(4) << std::setfill('0') << fraction;
	os << ','; 
	os << (isNorth ? 'N' : 'S');

	// std::cout << "Lat: ";
	// std::cout << std::setw(2) << std::setfill('0') << degrees;
	// std::cout << ":" ; 
	// std::cout << std::setw(2) << std::setfill('0') << minutes;
	// std::cout << '.';
	// std::cout << std::setw(4) << std::setfill('0') << fraction;
	// std::cout << std::endl;

	return os;
}

// Converts long to appropriate degrees and E/W
std::ostream& NMEAData::convertLong(std::ostream& os, double longDegrees) const {

	int degrees, minutes, fraction;

	bool isEast = longDegrees >= 0.0;
	double absLong = std::abs(longDegrees);

	// Longditude, first 3 digits are whole degrees (+/- 180 degrees east or west).
	degrees = (int)std::floor(absLong);
	absLong -= degrees;  // now fractional degrees.
	absLong *= 60;			// now minutes of arc.
	minutes = (int)std::floor(absLong);
	absLong -= minutes;		// decimal fraction of minute of arc.
	fraction = (int)std::floor(absLong * 10000);
	os << std::setw(3) << std::setfill('0') << degrees;
	os << std::setw(2) << std::setfill('0') << minutes;
	os << '.';
	os << std::setw(4) << std::setfill('0') << fraction;
	os << ','; 
	os << (isEast ? 'E' : 'W');

	// std::cout << "Long: ";
	// std::cout << std::setw(3) << std::setfill('0') << degrees;
	// std::cout << ":" ; 
	// std::cout << std::setw(2) << std::setfill('0') << minutes;
	// std::cout << '.';
	// std::cout << std::setw(4) << std::setfill('0') << fraction;
	// std::cout << std::endl;
	return os;
}

// Converts the UTC time to hhmmss.mmm
std::ostream& NMEAData::convertTime(std::ostream& os, double secondsFromMidnightUtc) const {

	double utc = secondsFromMidnightUtc;

	int h = int(std::floor(utc / (60 * 60)));
	utc -= h * (60 * 60);  // take off whole hours

	int m = int(std::floor(utc / 60));
	utc -= m * 60;	// take off mins, just secs left.

	int s = int(std::floor(utc));
	utc -= s;

	int mS = int(std::floor(utc * 1000)); // milliseconds

	os << std::setw(2) << std::setfill('0') << h;
	os << std::setw(2) << std::setfill('0') << m;
	os << std::setw(2) << std::setfill('0') << s;
	os << '.' ;
	os << std::setw(3) << std::setfill('0') << mS;

	return os;
}


// Wraps the basic sentence by writing the leading $, *, checksum and CRLF
void NMEAData::wrap(std::ostream& os, const std::string& in) const {
	os << "$";
	os << in;
	os << "*";
	os << std::hex << std::uppercase << std::setw(2) << std::setfill('0');
	os << checksum(in);
	os << std::dec << std::setw(0);
	os << "\r\n";
}

// Calculates the checksum for the core string.
unsigned int NMEAData::checksum(const std::string& in) const {

	int len = (int)in.length();
	unsigned int cs = 0;
	for(int i=0; i<len; ++i) {
		unsigned char ch = in.at(i);
		cs ^= ch;
	}

	return cs;
}

// Writes the GPS GGA sentence to the output.
void NMEAData::GPGGA(std::ostream& os, double utcTime, double latDegrees, double longDegrees, float gpsHeight){
	//std::cout << "GPS Height " << gpsHeight << std::endl;

	std::ostringstream oss;

	oss << "GPGGA,";

	convertTime(oss, utcTime) << ",";
	convertLat(oss, latDegrees) << ",";
	convertLong(oss, longDegrees) << ",";

	oss << "1,";  // GPS fix
	oss << "12,"; // number of satellites
	oss << "10,"; // Dilution of Position (DoP)
	oss << std::setprecision(1) << std::fixed << gpsHeight << ',';
	oss << heightUnits << ","; // in selected units
	oss << ",,,,"; // height of geoid, units, DGPS time, DGPS station
	oss << "0000";

	wrap(os, oss.str());
}



void NMEAData::GPRMC(std::ostream& os, double utcTime, double latDegrees, double longDegrees, float groundSpeedKnots, float trackDegrees, int day, int month, int year, float magneticVariationDegrees){
	std::ostringstream oss;

	oss << "GPRMC,";
	convertTime(oss, utcTime);
	oss << ",A,";  // valid

	convertLat(oss, latDegrees) << ",";

	convertLong(oss, longDegrees) << ",";

	oss << std::setw(0) << std::setprecision(2) << std::fixed;
	oss << groundSpeedKnots << ",";
	oss << trackDegrees << ",";

	oss << std::setw(2) << std::setfill('0');
	oss << int(day) << int(month) << int(year)%100 << ",";

	oss << std::setw(0) << std::fixed << std::setprecision(3) << std::abs(magneticVariationDegrees) << ",";
	oss << ((magneticVariationDegrees > 0) ? "E" : "W") << ",";  // 

	wrap(os, oss.str());
}


void NMEAData::PGRMZ(std::ostream& os, int altitude){
	std::ostringstream oss;
	//std::cout << "Altitude " << altitude << std::endl;
	oss << "PGRMZ,";
	oss << altitude << ",";
	oss << heightUnits << ",2";  // valid
	wrap(os, oss.str());
}

void NMEAData::PFLAU(std::ostream& os, int rx, int gps, int alarm, int relativeBearing, int alarmType, int relativeVertical, int relativeDistance, bool isICAO, uint32_t id){
	std::ostringstream oss;
	oss << "PFLAU,";
	if(rx < 0) rx = 0;
	if(rx > 99) rx = 99;
	oss << rx << ",";						// RX
	oss << ((gps != 0) ? 1 : 0) << ",";  	// TX only transmit when gps good.
	oss << gps << ",";						// GPS
	oss << 1 << ",";						// Power (ok)
	oss << alarm << ",";					// Alarm
	if(relativeBearing != NMEAData::EMPTY) { // Relative Bearing
		oss << std::setw(3) << std::setfill('0');
		oss << relativeBearing;
		oss << std::setw(0);
	} 
	oss << ",";
	oss << std::setw(1) << alarmType << ",";  // Alarm Type
	oss << std::setw(0);
	if(relativeVertical != NMEAData::EMPTY) { // Relative Vertical
		oss << relativeVertical;
	}
	oss << ",";
	if(relativeDistance != NMEAData::EMPTY) { // Relative Distance
		oss << relativeDistance;
	}
	oss << ",";
	if(isICAO){								  // ICAO address
		oss << std::setw(6) << std::setfill('0') << std::hex;
		oss << id;
		oss << std::setw(0) << std::dec;
	}

	wrap(os, oss.str());

}


// PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,
// <RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,
// <ClimbRate>,<AcftType>[,<NoTrack>][,<Source>,<RSSI>]
void NMEAData::PFLAA(std::ostream& os, int alarm, int relativeNorth, int relativeEast, int relativeVertical, bool isICAO, uint32_t id, int track, int groundSpeed, float climbRate, char acType){
	std::ostringstream oss;
	oss << "PFLAA,";
	oss << alarm << ",";
	oss << relativeNorth << ",";
	oss << relativeEast << ",";
	oss << relativeVertical << ",";
	if(isICAO){
		oss << std::setw(6) << std::setfill('0') << std::hex;
		oss << id;
		oss << std::setw(0) << std::dec;
	}	
	oss << ",";
	oss << track << ",";
	oss << ",";  // Turn rate: "Currently this field is empty"
	oss << groundSpeed << ",";
	oss << std::setprecision(1);
	oss << climbRate << ",";
	oss << acType;

	wrap(os, oss.str());

}


NMEAData::NMEAData(void)
{
}


NMEAData::~NMEAData(void)
{
}
