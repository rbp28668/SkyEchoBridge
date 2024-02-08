#include <iostream>
#include "frame.h"
#include "message.h"
#include "fcs.h"

    Frame::Frame()
    : used(0)
    {

    }
    
 
 void Frame::wrap(Message* message){

    size_t idx = 0;
    buffer[idx++] = 0x7E; // start of message.

    const uint8_t* msg = message->data();
    for(size_t i=0; i<message->length(); ++i){
        uint8_t b = msg[i];
        if(b == 0x7E || b == 0x7D){
            buffer[idx++] = 0x7D;  // DLE / ControlEscape
            buffer[idx++] = b ^ 0x20;
        } else {
            buffer[idx++] = b;
        }
    }
    // The FCS is a 16-bit CRC with the least significant byte first.
    uint16_t fcs = FCS::compute(message->data(), message->length());
    buffer[idx++] = fcs & 0xFF;
    buffer[idx++] = (fcs & 0xFF00) >> 8;

    buffer[idx++] = 0x7E; // end of message.
    
    used = idx;
 }