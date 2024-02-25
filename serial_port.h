#pragma once

#include <unistd.h> 
#include <fcntl.h> 
#include <termios.h> 
#include <errno.h>
#include <cstdint>
#include <string>

class SerialPort {

    int _fd;
    int _err;
    bool _bad;
    int _bps;
    std::string _error;

    static int  baudRates[];
    int lookupBaud(int bps);

    public:
    SerialPort(const char* device, int bps = 19200);
    ~SerialPort();

    inline int fd() { return _fd;}

    int bps() const { return _bps;}
    void setBps(int bps);
    int bytesWaiting();
    void waitUntilSent();
    int write(const uint8_t* data, size_t len);
    int read(uint8_t* buffer, size_t max);


};