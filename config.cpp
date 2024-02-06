#include "config.h"

// Use defaults for the time being.
Config::Config() 
    : _oldTarget(30)
    
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