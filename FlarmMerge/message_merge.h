#pragma once

#include <vector>
#include <list>
#include <stdlib.h>
#include "flarm_receiver.h"

class FlarmMessage;

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

   
    FlarmMessage *secondaryHeartbeat;
    std::vector<FlarmMessage *> traffic;
    std::list<FlarmMessage *> send_queue;

    PrimaryReceiver primaryReceiver;
    SecondaryReceiver secondaryReceiver;

    void receiveFlarm(FlarmMessage *msg);
    void receiveSecondary(FlarmMessage *msg);
    void send(FlarmMessage *msg);

public:
    MessageMerge();
    void receivePrimaryData(uint8_t *data, size_t nbytes);
    void receiveSecondaryData(uint8_t *data, size_t nbytes);
    FlarmMessage* allocateMessage(const char* data, size_t len);
};