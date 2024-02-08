#pragma once

#include <cstdint>
#include <stddef.h>

class Message;

class Frame {

    uint8_t buffer[1024]; // Big enough for any GDL-90 message even if well stuffed.
    size_t used;

    public:
    Frame();

    size_t length() const {return used;}
    const uint8_t* data() const {return buffer;}

    void wrap(Message* message);
};