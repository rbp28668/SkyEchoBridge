#include "transport.h"
#include <iostream>

Transport::Transport(MessageHandler* handler)
: handler(handler)
{

}

void Transport::process(uint8_t* buffer, size_t len){
    size_t pos = 0;
    size_t end = 0;
    int idx = 0; // message index for debug
    while(pos < len) {
        // Find the first flag character
        while((pos < len) && (buffer[pos] != Flag)) ++pos;
        ++pos; // skip initial flag
        if(pos >= len) return;
        
        size_t start = pos;   
        end = start;

        // Process body of message, unstuffing bytes as we go.
        while( (pos < len) && (buffer[pos] != Flag)){
            uint8_t b = buffer[pos];
            if(b == ControlEscape){
                ++pos; // skip ControlEscape char
                if(pos == len) break; // just in case
                b = buffer[pos] ^ 0x20;
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

