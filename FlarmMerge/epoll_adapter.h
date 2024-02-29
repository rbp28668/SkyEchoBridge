#pragma once
#include <cstdint>
#include <sys/epoll.h>

class EpollAdapter {
    public:
    virtual void onEvent(uint32_t events) = 0;
};
