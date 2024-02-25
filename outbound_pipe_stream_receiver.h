#pragma once
#include "stream_receiver.h"
#include "outbound_pipe.h"

/// @brief Receives data from an output stream adapter
class OutboundPipeStreamReceiver : StreamReceiver {

    OutboundPipe pipe;

    public:
    OutboundPipeStreamReceiver(const char *fifo) : pipe(fifo) {}
    inline int fd() { return pipe.fd(); }
     virtual bool receive(const char* data, size_t length);
};