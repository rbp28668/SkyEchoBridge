#pragma once
#include <cstdint>
#include <stdlib.h>

class InboundPipe
{

    int _fd;

public:
    InboundPipe(const char *fifo);
    ~InboundPipe();
    inline int fd() { return _fd; }
    int read(uint8_t *buffer, size_t max);
    bool valid() const { return _fd != -1;}
};