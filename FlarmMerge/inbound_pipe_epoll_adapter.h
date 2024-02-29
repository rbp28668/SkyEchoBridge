#pragma once

#include "epoll_adapter.h"
#include "message_merge.h"
#include "inbound_pipe.h"

class InboundPipeEpollAdapter : public EpollAdapter{
    
    MessageMerge* merger;
    InboundPipe* pipe;
    public:
        InboundPipeEpollAdapter(MessageMerge* merger, InboundPipe* pipe);
        virtual void onEvent(uint32_t events);
};
