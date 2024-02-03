// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <cstdint>
#include <stddef.h>
#include "message_handler.h"

/// @brief Manages the framing and byte stuffing to
/// convert the byte stream into a message stream.
class Transport{

    const uint8_t Flag = 0x7E;
    const uint8_t ControlEscape = 0x7D;
    MessageHandler* handler;
public:
    Transport(MessageHandler* handler);
    void process(uint8_t* buffer, size_t len); 
};