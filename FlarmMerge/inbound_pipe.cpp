#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

#include "inbound_pipe.h"
#include <cerrno>

InboundPipe::InboundPipe(const char *fifo)
    : _fd(-1)
{
    _fd = ::open(fifo, O_RDONLY | O_NONBLOCK);
    if(_fd == -1){
        std::cerr << "Unable to open pipe " << fifo << ": " << strerror(errno) << std::endl;
    }
}

InboundPipe::~InboundPipe()
{
    if (_fd != -1)
    {
        ::close(_fd);
    }
}

// If any process has the pipe
// open for writing, reads fail with EAGAIN; otherwise—with no
// potential writers—reads succeed and return empty.

int InboundPipe::read(uint8_t *buffer, size_t max)
{
    int bytes = ::read(_fd, buffer, max);
    return bytes;
}