#pragma once

#include "epoll_adapter.h"
#include "message_merge.h"
#include "../serial_port.h"

class SerialEpollAdapter : public EpollAdapter, FlarmMessageWriter{
    
    MessageMerge* merger;
    SerialPort* serial;

    uint8_t buffer[256]; // read buffer

    FlarmMessage* outbound;
    int sent;

    void sendData();
public:
    SerialEpollAdapter(MessageMerge* merger, SerialPort* serial);
    virtual void onEvent(uint32_t events);
    virtual void send(MessageMerge* merger);
};
