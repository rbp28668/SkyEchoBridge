#include <assert.h>
#include <string>
#include <iostream>
#include "serial_epoll_adapter.h"
#include "flarm_message.h"

SerialEpollAdapter::SerialEpollAdapter(MessageMerge *merger, SerialPort* serial)
: merger(merger)
, serial(serial)
, outbound(nullptr)
, sent(0)
{
    assert(this);
    assert(merger);
    assert(serial);
}

void SerialEpollAdapter::sendData() {

    // Get first thing to send if nothing part sent.
    if(outbound == nullptr){
        outbound = merger->getOutboundMessage();
        sent = 0;
    }

    while(outbound != nullptr){
        const char* data = outbound->data() + sent;
        int len = outbound->length() - sent;

        int nbytes = serial->write( reinterpret_cast<const uint8_t*>(data), len);
        if(nbytes > 0){
            sent += nbytes;
        } else if(nbytes == -1) {
            if(errno == EAGAIN){  // Would block...
                break;  // so stop processing - epoll should fire when available again.
            }
        }

        if(sent == outbound->length()){
            merger->messageConsumed(outbound);
            outbound = merger->getOutboundMessage(); // next message?
            sent = 0;
        }

    }
}

void SerialEpollAdapter::onEvent(uint32_t events)
{
    assert(this);
    if(events & EPOLLIN){
        int nbytes = serial->read(buffer, sizeof(buffer));
        if(nbytes >= 0){
            merger->receivePrimaryData(buffer, nbytes);
            std::cout << "Serial RX: " << std::string(reinterpret_cast<char*>(buffer), nbytes) << std::endl;
        }
    }

    if(events & EPOLLOUT){
        sendData();
    }

}

void SerialEpollAdapter::send(MessageMerge* pmm){
    std::cout << "Serial TX" << std::endl;
    sendData();   // or try to at least.
}