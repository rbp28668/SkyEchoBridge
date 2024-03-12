// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once
#include <limits>
#include <ostream>
#include <ctype.h>

class NMEAData
{
public:
	// double latDegrees;		// North is positive, South negative
	// double longDegrees;		// East is positive, West negative
	// double gpsHeight;		// in metres
	// double utcTime;			// in seconds from midnight;
	// double groundSpeedKnots;
	// double trackDegrees;
	// double magneticVariationDegrees;
	// double IASkph;
	// double baroAltitudeMetres;
	// double varioMetresSec;
	// double headingDegrees;
	// double windHeadingDegrees;
	// double windSpeedKph;
	// double year;
	// double month;
	// double day;

private:
	std::ostream& convertLat(std::ostream& os, double latDegrees) const;
	std::ostream& convertLong(std::ostream& os, double longDegrees) const;
	std::ostream& convertTime(std::ostream& os, double secondsFromMidnightUtc) const;

	void wrap(std::ostream& os, const std::string& in) const;

	unsigned int checksum(const std::string& in) const;

public:
	static constexpr int EMPTY = std::numeric_limits<int>::max();
	static constexpr char heightUnits = 'F';

	void GPGGA(std::ostream& os, double utcTime, double latDegrees, double longDegrees, float gpsHeight);
	void GPRMC(std::ostream& os, double utcTime, double latDegrees, double longDegrees, float groundSpeedKnots, float trackDegrees, int day, int month, int year, float magneticVariationDegrees);
	void PGRMZ(std::ostream& os, int altitude);
	void PFLAU(std::ostream& os, int rx, bool tx, int gps, int alarm, int relativeBearing, int alarmType, int relativeVertical, int relativeDistance, bool isICAO, uint32_t id);
	void PFLAA(std::ostream& os, int alarm, int relativeNorth, int relativeEast, int relativeVertical, bool isICAO, uint32_t id, int track, int groundSpeed, float climbRate, char acType);

	NMEAData(void);
	~NMEAData(void);
};

