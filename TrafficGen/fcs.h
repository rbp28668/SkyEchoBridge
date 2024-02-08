// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <cstdint>
#include <stddef.h>
/// Frame Check Sequence
class FCS {
    static uint16_t lookup[256];

    public:

    static uint16_t compute(uint8_t *block, size_t length);
};