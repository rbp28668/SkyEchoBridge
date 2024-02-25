#pragma once
#include <cstdint>
#include <stdlib.h>

/// @brief Class to assemble a full NMEA message from various bits 
/// of data such as might be received from a serial port.  Manages
/// the framing of the message and calls onReceive when a complete
/// message has been processed.  Copes with partial messages and
/// multiple messages in one call to receive.
class FlarmReceiver {

    char buffer[1024];
    int state;
    int offset; 
    
    bool checkBuffer();
    void reset() {state = offset = 0;}

    public:
    // Call this to pass data into the receiver.  Doesn't
    // have to be a complete message.
    void receive(uint8_t* buffer, size_t nbytes);

    // this is called when this has a complete NMEA message.
    virtual void onReceive(const char* data, size_t nbytes) = 0;

};