#include <assert.h>
#include "inbound_pipe_epoll_adapter.h"

InboundPipeEpollAdapter::InboundPipeEpollAdapter(MessageMerge *merger, InboundPipe *pipe)
: merger(merger)
, pipe(pipe)
{
    assert(this);
    assert(merger);
    assert(pipe);
}

void InboundPipeEpollAdapter::onEvent(uint32_t events)
{
    assert(this);
    uint8_t buffer[256];
    if(events & EPOLLIN){
        int nbytes = pipe->read(buffer, sizeof(buffer));
        if(nbytes >= 0){
            merger->receiveSecondaryData(buffer, nbytes);
        }
    }

}
