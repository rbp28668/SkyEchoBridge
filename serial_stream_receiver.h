#pragma once
#include "stream_receiver.h"
#include "serial_port.h"

/// @brief Receives data from an output stream adapter
class SerialStreamReceiver : public StreamReceiver {

    SerialPort serial;

    public:
    SerialStreamReceiver(const char* device, int bps = 19200) : serial(device, bps) {}
    inline int fd() { return serial.fd(); }
    inline bool valid() const { return serial.valid();}

    virtual bool receive(const char* data, size_t length);
};