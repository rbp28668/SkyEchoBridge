// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once
#include <cstdint>
#include <stddef.h>

class Message
{
    public:
    virtual size_t length() = 0;
    virtual uint8_t* data() = 0;
};