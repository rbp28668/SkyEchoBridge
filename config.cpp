#include "config.h"

// Use defaults for the time being.
Config::Config() 
    : _listenPort(4000)
    , _outputType(Config::OutputType::Console)
    
    , _targetIp("192.168.0.100")
    , _targetPort(4353)

    , _serialDevice("/dev/serial0")
    , _baudRate(38400)

    , _fifo("/opt/sky_echo/sky_echo_fifo")

    , _oldTarget(30)
    
    , _ignoreDistance(10000)
    , _ignoreHeight(500)

    , _adviseDistance(1500)
    , _adviseHeight(300)

    , _threatDistance(500)
    , _threatHeight(100)
    , _threatTime(30)

    , _threatBubbleDistance(2500)
    , _threatBubbleHeight(500)
{

}