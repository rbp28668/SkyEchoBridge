#include <assert.h>
#include <string>
#include <iostream>
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
            //std::cout << "Pipe RX: " << std::string(reinterpret_cast<char*>(buffer), nbytes) << std::endl;
        }
    }

}
