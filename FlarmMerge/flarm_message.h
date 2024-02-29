#pragma once
#include <stdint.h>

class FlarmMessage {

    char buffer[256];
    int len;

    bool _valid;
    int _alarmLevel;
    bool _isTraffic;
    bool _isHeartbeat;
    bool _hasAdvisory;
    uint32_t _id;

    bool startsWith(const char* str);
    const char* getField(int idx);
    bool checkValid();
    int extractAlarmLevel();
    bool extractAdvisory();
    uint32_t extractId();
    
    public:

    FlarmMessage(const char* data, size_t len);
    bool isFlarm();     // Flarm specific message.
    bool isValid()  { return _valid; }
    int alarmLevel() { return _alarmLevel; }
    bool hasAdvisory() { return _hasAdvisory; }
    bool isTraffic()  const { return _isTraffic; } // PFLAA
    bool isHeartbeat() const { return _isHeartbeat;} // PFLAU;
    uint32_t getId() {return _id; }  // address and address type combined.

    // Get the message for sending.
    const char* data() const {return buffer;}
    int length() const { return len;}
};