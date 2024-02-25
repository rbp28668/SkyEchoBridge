#include <sys/ioctl.h>
#include <cstring>
#include "serial_port.h"

// want to know how many bps we're actually running at so can manage data flow.
// Hence expose actual rate to outside world and convert internally.
int SerialPort::baudRates[] = {
    1200, B1200,
    2400, B2400,
    4800, B4800,
    9600, B9600,
    19200, B19200,
    38400, B38400,
    57600, B57600,
    115200, B115200,
    0, 0};

SerialPort::SerialPort(const char *device, int bps)
    : _bps(bps), _err(0), _bad(false), _fd(-1)
{
    int speed = lookupBaud(bps);
    if (speed == 0)
    {
        _err = EINVAL;
        _bad = true;
        _error.assign("Invalid baud rate");
        return;
    }

    // Read/Write and don't kill if ctrl-C received. Also non-blocking mode
    _fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (_fd == -1)
    {
        _bad = true;
        _err = errno;
        _error.assign(strerror(errno));
    }
    else
    {
        struct termios options;

        if(tcgetattr(_fd, &options) == -1){
            _bad = true;
            _err = errno;
            _error.assign(strerror(errno));
        }
        cfsetspeed(&options, speed);

        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8; // 8 bits

        options.c_cflag &= ~PARENB; // No parity
        options.c_cflag &= ~CSTOPB; // 1 stop bit

        options.c_cflag |= CLOCAL; // Ignore modem control
        options.c_cflag |= CREAD;  // Enable read

        options.c_cc[VTIME] = 0; // Just return whatever data is available
        options.c_cc[VMIN] = 0;  // on read.

        cfmakeraw(&options);
        tcsetattr(_fd, TCSANOW, &options);
    }
}

SerialPort::~SerialPort()
{
    if(_fd != -1){
        ::close(_fd);
    }
}

/// @brief Lookup the baud rate code given an actual baud rate.
/// @param bps
/// @return baud rate code or 0 if not found.
int SerialPort::lookupBaud(int bps)
{
    int code = 0;
    for (int i = 0; baudRates[i] != 0; i += 2)
    {
        if (baudRates[i] == bps)
        {
            code = baudRates[i + 1];
            break;
        }
    }
    return code;
}

void SerialPort::setBps(int bps)
{
    int speed = lookupBaud(bps);
    if (speed != 0)
    {
        _bps = bps;
        struct termios options;
        tcgetattr(_fd, &options);
        cfsetspeed(&options, speed);
        tcsetattr(_fd, TCSANOW, &options);
    } else {
        _err = EINVAL;
        _bad = true;
        _error.assign("Invalid baud rate");
    }
}

int SerialPort::bytesWaiting()
{
    int bytes;
    ioctl(_fd, FIONREAD, &bytes);
    return bytes;
}

void SerialPort::waitUntilSent()
{
    tcdrain(_fd);
}

int SerialPort::write(const uint8_t *data, size_t len)
{
    return ::write(_fd, data, len);
}

int SerialPort::read(uint8_t *buffer, size_t max)
{
    return ::read(_fd, buffer, max);
}