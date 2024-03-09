#pragma once
#include <cstdint>
#include <stdlib.h>

class OutboundPipe
{

    int _fd;
    bool _disconnected;
public:
    OutboundPipe(const char *fifo);
    ~OutboundPipe();
    inline int fd() { return _fd; }
    inline bool disconnected() const { return _disconnected;}
    bool valid() const { return _fd != -1;}

    int write(const uint8_t *data, size_t len);
};