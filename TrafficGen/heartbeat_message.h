#pragma once
#include "message.h"

class HeartbeatMessage : public Message
{

    static constexpr int MESSAGE_LENGTH = 7;
    uint8_t message[MESSAGE_LENGTH];

    public:
    bool gpsAvailable;
    bool maintRequired;
    bool batteryLow;
    bool utcOk;
    uint32_t ts;
    uint32_t receivedMsgCount;

    HeartbeatMessage();

    virtual size_t length();
    virtual uint8_t* data();

    void build();
};