// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#pragma once

class Config {

    int _oldTarget;
    
    int _ignoreDistance;
    int _ignoreHeight;

    int _adviseDistance;
    int _adviseHeight;

    int _threatDistance;
    int _threatHeight;
    int _threatTime;
    
    int _threatBubbleDistance;
    int _threatBubbleHeight;

    public:

    Config();

    // If a target hasn't been received for this time stop tracking.
    int oldTarget() const  { return _oldTarget;}

    // If a target is more than this distance away or has more than this 
    // vertcal separation then ignore it.  Defaults 10,000m / 500m
    int ignoreDistance() const { return _ignoreDistance; } 
    int ignoreHeight() const { return _ignoreHeight; } 

    // If a target is within this distance and height then look to emit an advisory. (1500 / 300)
    int adviseDistance() const { return _adviseDistance; } 
    int adviseHeight() const { return _adviseHeight; } 

    // These define a threat bubble around the ownship.  
    // If distance = height = time = 0 then threat is 1.0. Any value outside the
    // bubble is 0 and reduces proportionally as the target gets to the edge. 
    int threatDistance() const { return _threatDistance; }  // Default 500m
    int threatHeight() const { return _threatHeight; }      // Default 100m
    int threatTime() const { return _threatTime; }          // Default 30s

    // If the target is within this bubble then look to calculate a threat score.
    int threatBubbleDistance() const { return _threatBubbleDistance; } // Default 2500m
    int threatBubbleHeight() const { return _threatBubbleHeight; }      // Default 500m
};