#include "outbound_pipe_stream_receiver.h"

bool OutboundPipeStreamReceiver::receive(const char *data, size_t length)
{
    return pipe.write((uint8_t*) data, length) == length;
}
