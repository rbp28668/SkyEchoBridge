// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#pragma once
#include <string>

class Config {

public:
    // output type
    enum class OutputType{
        Console,
        Socket,
        Serial,
        Pipe
    };

private:

    // Listen port for SkyEcho messages
    unsigned int _listenPort;

    // Where to send the output
    OutputType _outputType;

    // Params for output via UDP
    std::string _targetIp;
    uint _targetPort;

    // Params for output via serial port
    std::string _serialDevice;
    int _baudRate;

    // Params for output via named pipe.
    std::string _fifo;

    // Suppress tx flag if set
    bool _notx;

    // Algorithm parameters
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

    bool isOption(const std::string& arg, const std::string& option);
    bool requiredParam(std::string &param, char *argv[], int idx, int argc, const char *err);
    bool requiredParam(int &param, char *argv[], int idx, int argc, const char *err);
    bool optionalParam(std::string &param, char *argv[], int idx, int argc);
    bool optionalParam(int &param, char *argv[], int idx, int argc);

    public:

    Config();

    bool update(int argc, char* argv[]);

    // Listen for SkyEcho on this port.
    uint listenPort() const { return _listenPort;}

    // Where to send the output FLARM messages
    Config::OutputType outputType() const { return _outputType;}
    
    // IP address to send Socket UDP output to.
    const std::string& targetIp() const {return  _targetIp;}
    // Port to send Socket UDP output to.
    unsigned int targetPort() const {return _targetPort;}

    // Which serial device to use to send data to.
    const std::string& serialDevice() const { return _serialDevice;}
    // baud rate to set serial device to.
    const int baudRate() const {return  _baudRate;}

    // location of fifo / named pipe (use mkfifo to create).
    const std::string& fifo() const { return _fifo;}

    // If true, set the tx flag to zero on the output PFLAU.
    // Allows indication that primary isn't transmitting if displaying secondary data.
    const bool NoTx() const { return _notx;}

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