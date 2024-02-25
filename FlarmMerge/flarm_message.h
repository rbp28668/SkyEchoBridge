#pragma once
#include <stdint.h>

class FlarmMessage {

    char buffer[256];

    bool valid;
    bool validityChecked;

    bool startsWith(const char* str);
    const char* getField(int idx);

    public:

    FlarmMessage(const char* data, size_t len);
    bool isFlarm();     // Flarm specific message.
    bool isValid();
    int alarmLevel();
    bool hasAdvisory();
    bool isTraffic(); // PFLAA
    bool isHeartbeat(); // PFLAU;
    uint32_t getId();  // address and address type combined.
};