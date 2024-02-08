#pragma once

#include <time.h>
#include <cstdint>

class Clock {
    public:
    static uint32_t time() { 
        time_t rawtime = ::time(nullptr);
        struct tm* ptm = gmtime ( &rawtime );
        return (ptm->tm_hour * 60 + ptm->tm_min) * 60 + ptm->tm_sec;
    }
};