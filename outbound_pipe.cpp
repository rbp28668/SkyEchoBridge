#include "outbound_pipe.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cerrno>

// https://www.man7.org/linux/man-pages/man7/pipe.7.html

OutboundPipe::OutboundPipe(const char *fifo)
    : _fd(-1), _disconnected(false)
{
    _fd = ::open(fifo, O_WRONLY | O_NONBLOCK);
    signal(SIGPIPE, SIG_IGN); // ignore broken pipe - write will return EPIPE error if no readers.
}

OutboundPipe::~OutboundPipe()
{
    if (_fd != -1)
    {
        ::close(_fd);
    }
}

int OutboundPipe::write(const uint8_t *data, size_t len)
{
    int bytes = ::write(_fd, data, len);
    _disconnected = (bytes == EPIPE); // No readers with open descriptors.
    if (_disconnected)
    {
        bytes = 0;
    }
    return bytes;
}