#pragma once

#include <vector>
#include <list>
#include <stdlib.h>
#include "flarm_receiver.h"

class FlarmMessage;
class MessageMerge;

/// @brief Interface to allow signalling that there is data to send.
struct FlarmMessageWriter {
    virtual void send(MessageMerge* pmm) = 0;
};

/// @brief This is the class that manages bringing the flarm (primary) and
/// ADSB (secondary) data together.
class MessageMerge
{

    /// @brief Shim class to receive data and pass it on as primary (flarm) data.
    class PrimaryReceiver : public FlarmReceiver
    {
        MessageMerge *mm;
        virtual void onReceive(const char *data, size_t nbytes);
        public:
        PrimaryReceiver(MessageMerge *mm) : mm(mm) {}
    };

    /// @brief Shim class to receive data and pass it on as secondary(ADSB derived) data.
    class SecondaryReceiver : public FlarmReceiver
    {
        MessageMerge *mm;
        virtual void onReceive(const char *data, size_t nbytes);
        public:
        SecondaryReceiver(MessageMerge *mm) : mm(mm) {}
    };

    FlarmMessage *primaryHeartbeat = nullptr;     // From FLARM
    FlarmMessage *secondaryHeartbeat = nullptr;   // From ADSB
    std::vector<FlarmMessage *> primaryTraffic; 
    std::vector<FlarmMessage *> secondaryTraffic;
    
    // Incremented by receiving secondary heartbeat, zeroed 
    // when receiving primary.  If > N (say 3) then primary
    // is inactive but we're getting secondary information.
    int secondaryCount = 0;
    bool secondaryActive() const { return secondaryCount > 3;}

    bool hasPrimaryFix = false;   // set true if the primary heartbeat has valid GPS.

    std::list<FlarmMessage *> send_queue;
    FlarmMessageWriter* writer = nullptr;

    PrimaryReceiver primaryReceiver;
    SecondaryReceiver secondaryReceiver;

    void receiveFlarm(FlarmMessage *msg);
    void receiveSecondary(FlarmMessage *msg);
    void sendHeartbeat(); // manage logic to decide which heartbeat to send.
    void sendTraffic();   // And traffic 
    void send(FlarmMessage *msg);

public:
    MessageMerge(FlarmMessageWriter* writer = nullptr);
    void setWriter(FlarmMessageWriter* writer);
    void receivePrimaryData(uint8_t *data, size_t nbytes);
    void receiveSecondaryData(uint8_t *data, size_t nbytes);
    FlarmMessage* allocateMessage(const char* data, size_t len);

    // To manage send queue:
    FlarmMessage* getOutboundMessage();
    void messageConsumed(FlarmMessage* msg);
};