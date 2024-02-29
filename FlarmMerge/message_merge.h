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

class MessageMerge
{

    class PrimaryReceiver : public FlarmReceiver
    {
        MessageMerge *mm;
        virtual void onReceive(const char *data, size_t nbytes);
        public:
        PrimaryReceiver(MessageMerge *mm) : mm(mm) {}
    };

    class SecondaryReceiver : public FlarmReceiver
    {
        MessageMerge *mm;
        virtual void onReceive(const char *data, size_t nbytes);
        public:
        SecondaryReceiver(MessageMerge *mm) : mm(mm) {}
    };

    FlarmMessage *primaryHeartbeat;     // From FLARM
    FlarmMessage *secondaryHeartbeat;   // From ADSB
    std::vector<FlarmMessage *> primaryTraffic; 
    std::vector<FlarmMessage *> secondaryTraffic;
    
    std::list<FlarmMessage *> send_queue;
    FlarmMessageWriter* writer;

    PrimaryReceiver primaryReceiver;
    SecondaryReceiver secondaryReceiver;

    void receiveFlarm(FlarmMessage *msg);
    void receiveSecondary(FlarmMessage *msg);
    void sendHeartbeat(); // manage logic to decide which heartbeat to send.
    void sendTraffic();   // And traffic 
    void send(FlarmMessage *msg);

public:
    MessageMerge(FlarmMessageWriter* writer = 0);
    void setWriter(FlarmMessageWriter* writer);
    void receivePrimaryData(uint8_t *data, size_t nbytes);
    void receiveSecondaryData(uint8_t *data, size_t nbytes);
    FlarmMessage* allocateMessage(const char* data, size_t len);

    // To manage send queue:
    FlarmMessage* getOutboundMessage();
    void messageConsumed(FlarmMessage* msg);
};