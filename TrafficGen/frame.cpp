#include <iostream>
#include "frame.h"
#include "message.h"
#include "fcs.h"

    Frame::Frame()
    : used(0)
    {

    }
    
 
 void Frame::wrap(Message* message){

    // The FCS is a 16-bit CRC with the least significant byte first.
    // It's calculated on the raw (unstuffed) message.
    uint16_t fcs = FCS::compute(message->data(), message->length());

    size_t idx = 0;
    buffer[idx++] = 0x7E; // start of message.

    const uint8_t* msg = message->data();

    // Copy message to buffer byte stuffing
    for(size_t i=0; i<message->length(); ++i){
        
        uint8_t b = msg[i];
    
        if((b == 0x7E) || (b == 0x7D)){
            buffer[idx++] = 0x7D;  // DLE / ControlEscape
            buffer[idx++] = b ^ 0x20;
        } else {
            buffer[idx++] = b;
        }
    }
 
    // Add 2 byte FCS ls byte first, also byte stuffed.
    for(int i=0; i<2; ++i){
        uint8_t b = fcs & 0xFF;
        if((b == 0x7E) || (b == 0x7D)){
            buffer[idx++] = 0x7D;  // DLE / ControlEscape
            buffer[idx++] = b ^ 0x20;
        } else {
            buffer[idx++] = b;
        }
        fcs >>= 8;
    }
 
    buffer[idx++] = 0x7E; // end of message.
    
    used = idx;
 }