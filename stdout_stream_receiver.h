#pragma once
#include "stream_receiver.h"

/// @brief Receives data from an output stream adapter
class StdoutStreamReceiver : StreamReceiver {
     virtual bool receive(const char* data, size_t length);
};