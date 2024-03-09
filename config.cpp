#include "config.h"
#include <iostream>

// Use defaults for the time being.
Config::Config()
    : _listenPort(4000), _outputType(Config::OutputType::Console)

      ,
      _targetIp("192.168.0.100"), _targetPort(4353)

      ,
      _serialDevice("/dev/serial0"), _baudRate(38400)

      ,
      _fifo("/opt/sky_echo/sky_echo_fifo")

      ,
      _oldTarget(30)

      ,
      _ignoreDistance(10000), _ignoreHeight(500)

      ,
      _adviseDistance(1500), _adviseHeight(300)

      ,
      _threatDistance(500), _threatHeight(100), _threatTime(30)

      ,
      _threatBubbleDistance(2500), _threatBubbleHeight(500)
{
}

bool Config::isOption(const std::string &arg, const std::string &option)
{
    return (arg.substr(0, 2) == "--") && (arg.substr(2, arg.length() - 2) == option);
}

bool Config::requiredParam(std::string &param, char *argv[], int idx, int argc, const char *err)
{
    if (idx >= argc)
    {
        std::cerr << err << std::endl;
        return false; // failed
    }

    std::string arg = argv[idx];
    if (arg.substr(0, 2) == "--")
    {
        std::cerr << err << std::endl;
        return false; // failed
    }

    param = arg;
    return true;
}

bool Config::requiredParam(int &param, char *argv[], int idx, int argc, const char *err)
{
    if (idx >= argc)
    {
        std::cerr << err << std::endl;
        return false; // failed
    }

    std::string arg = argv[idx];
    if (arg.substr(0, 2) == "--")
    {
        std::cerr << err << std::endl;
        return false; // failed
    }

    if(!isdigit(arg[0])) return false;

    param = atoi(arg.c_str());
    return true;
}

bool Config::optionalParam(std::string &param, char *argv[], int idx, int argc)
{
    if (idx >= argc)
    {
        return false; // no param
    }

    std::string arg = argv[idx];
    if (arg.substr(0, 2) == "--")
    {
        return false; // no param
    }

    param = arg;
    return true;
}

bool Config::optionalParam(int &param, char *argv[], int idx, int argc)
{
    if (idx >= argc)
    {
        return false; // no param
    }

    std::string arg = argv[idx];
    if (arg.substr(0, 2) == "--")
    {
        return false; // failed
    }

    if(!isdigit(arg[0])) return false;

    param = atoi(arg.c_str());
    return true;
}

/// @brief Updates the configuration from the command line values.
/// @param argc arg count from main.
/// @param argv args from main.
/// @return true for success, false if the command line is invalid.
bool Config::update(int argc, char *argv[])
{

    int idx = 1;
    while (idx < argc)
    {

        std::string arg = argv[idx++];

        if (isOption(arg, "help"))
        {
            std::cout << "SkyEcho 2 " << std::endl;
            std::cout << "--help                            - display this information." << std::endl;
            std::cout << "--listen <port>                   - listen for GDL-90 packets on this port" << std::endl;
            std::cout << "--console  (default)              - set output device to console (stdout)." << std::endl;
            std::cout << "--pipe <fifo>                     - set output device to named pipe." << std::endl;
            std::cout << "--serial <device> <baud>          - set output device to serial port." << std::endl;
            std::cout << "--udp <ip_address> <port>         - set output device to UDP socket" << std::endl;

            std::cout << "--old_target <value>              - time in seconds to keep target information without an update." << std::endl;
            std::cout << "--ignore_distance  <value>        - ignore targets further away than this (metres)." << std::endl;
            std::cout << "--ignore_height  <value>          - ignore targets with a larger height difference than this (metres)." << std::endl;
            std::cout << "--advise_distance  <value>        - distance within which to trigger a traffic advisory." << std::endl;
            std::cout << "--advise_height  <value>          - height difference within which to trigger a traffic advisory." << std::endl;
            std::cout << "--threat_distance  <value>        - threat score threshold horizontal distance" << std::endl;
            std::cout << "--threat_height  <value>          - threat score threshold vertical distance" << std::endl;
            std::cout << "--threat_time  <value>            - threat score threshold time to closest approach" << std::endl;
            std::cout << "--threat_bubble_distance  <value> - treat targets closer than this as a possible threat (metres)." << std::endl;
            std::cout << "--threat_bubble_height  <value>   - treat targets with a closer height difference as a possible threat (metres)." << std::endl;
            return false;
        }

        if (isOption(arg, "console"))
        {
            _outputType = Config::OutputType::Console;
        }
        else if (isOption(arg, "pipe"))
        {
            _outputType = Config::OutputType::Pipe;
            optionalParam(_fifo, argv, idx++, argc);
        }
        else if (isOption(arg, "serial"))
        {
            _outputType = Config::OutputType::Serial;
            if (optionalParam(_serialDevice, argv, idx++, argc))
            {
                optionalParam(_baudRate, argv, idx++, argc);
            }
        }

        else if (isOption(arg, "udp"))
        {
            _outputType = Config::OutputType::Socket;
            if (optionalParam(_targetIp, argv, idx++, argc))
            {
                int port;
                if (optionalParam(port, argv, idx++, argc))
                {
                    _targetPort = (unsigned)port;
                }
            }
        }

        else if (isOption(arg, "old_target"))
        {
            if (!requiredParam(_oldTarget, argv, idx++, argc, "missing value for old_target"))
            {
                return false;
            }
        }

        else if (isOption(arg, "ignore_distance"))
        {
            if (!requiredParam(_ignoreDistance, argv, idx++, argc, "missing value for ignore_distance"))
            {
                return false;
            }
        }

        else if (isOption(arg, "ignore_height"))
        {
            if (!requiredParam(_ignoreHeight, argv, idx++, argc, "missing value for ignore_height"))
            {
                return false;
            }
        }

        else if (isOption(arg, "advise_distance"))
        {
            if (!requiredParam(_adviseDistance, argv, idx++, argc, "missing value for advise_distance"))
            {
                return false;
            }
        }

        else if (isOption(arg, "advise_height"))
        {
            if (!requiredParam(_adviseHeight, argv, idx++, argc, "missing value for advise_height"))
            {
                return false;
            }
        }

        else if (isOption(arg, "threat_distance"))
        {
            if (!requiredParam(_threatDistance, argv, idx++, argc, "missing value for threat_distance"))
            {
                return false;
            }
        }

        else if (isOption(arg, "threat_height"))
        {
            if (!requiredParam(_threatHeight, argv, idx++, argc, "missing value for threat_height"))
            {
                return false;
            }
        }

        else if (isOption(arg, "threat_time"))
        {
            if (!requiredParam(_threatTime, argv, idx++, argc, "missing value for threat_time"))
            {
                return false;
            }
        }

        else if (isOption(arg, "threat_bubble_distance"))
        {
            if (!requiredParam(_threatBubbleDistance, argv, idx++, argc, "missing value for threat_bubble_distance"))
            {
                return false;
            }
        }

        else if (isOption(arg, "threat_buble_height"))
        {
            if (!requiredParam(_threatBubbleHeight, argv, idx++, argc, "missing value for threat_buble_height"))
            {
                return false;
            }
        }
    }
    return true;
}