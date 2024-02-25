#include "serial_stream_receiver.h"

bool SerialStreamReceiver::receive(const char *data, size_t length)
{
    return serial.write((uint8_t*) data, length) == length;
}
