// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include "transport.h"
#include <iostream>
#include <iomanip>
#include <assert.h>

/// @brief Creates a message transport bound to a given handler
/// which will receive messages as they arrive.
/// @param handler will have its onMessage method called for each
/// received message.
Transport::Transport(MessageHandler* handler)
: handler(handler)
{

}

/// @brief Process the memory buffer looking for GDL-90 messages.
/// Will call the message handler's onMessage method for every
/// message found.
/// @param buffer is the data to process.
/// @param len the number of bytes in the buffer to process.
void Transport::process(uint8_t* buffer, size_t len){
    size_t pos = 0;
    size_t end = 0;
    int idx = 0; // message index for debug

    while(pos < len) {
        // Find the first flag character
        while((pos < len) && (buffer[pos] != Flag)) ++pos;
        ++pos; // skip initial flag
        if(pos >= len) return; // No message delimiters in the buffer.
        
        size_t start = pos;   // First character of message
        end = start;          // Where to copy characters to.

        // Process body of message, unstuffing bytes as we go.
        while( (pos < len) && (buffer[pos] != Flag)){
            uint8_t b = buffer[pos];
            if(b == ControlEscape){
                ++pos; // skip ControlEscape char
                if(pos == len) {
                    break; // just in case
                }
                b = buffer[pos] ^ 0x20;
                assert(b == 0x7D || b == 0x7E);  // as these are the only 2 that should be escaped.
            }
            buffer[end] = b;
            ++end;
            ++pos;
        }
        ++pos; // skip end of message flag

        ++idx;
        // std::cout << "Message " << idx << std::endl;
        // Ok, so at this point: 
        // start should be the index of the first character of the message
        // end should be the index first character past the message
        handler->onMessage(buffer + start, end - start);
    }


} 

