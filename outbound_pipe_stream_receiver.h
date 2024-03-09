#pragma once
#include "stream_receiver.h"
#include "outbound_pipe.h"

/// @brief Receives data from an output stream adapter
class OutboundPipeStreamReceiver : public StreamReceiver {

    OutboundPipe pipe;

    public:
    OutboundPipeStreamReceiver(const char *fifo) : pipe(fifo) {}
    inline int fd() { return pipe.fd(); }
    inline bool valid() const { return pipe.valid();}
    virtual bool receive(const char* data, size_t length);
};