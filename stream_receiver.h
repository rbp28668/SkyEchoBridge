#pragma once
#include <stddef.h>

/// @brief Receives data from a StreamAdapter
class StreamReceiver {
    public:
    /// @brief Writes a data packet to the output
    /// @param data 
    /// @param length 
    /// @return true for success, false for failure.
    virtual bool receive(const char* data, size_t length) = 0;
};
