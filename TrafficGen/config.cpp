#include "config.h"
#include <iostream>
#include <cctype>


Config::Config()
    : 
      _targetIp("127.0.0.1"), _targetPort(4000)
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
        return false; // not a param
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
            std::cout << "TargetGen " << std::endl;
            std::cout << "--help                            - display this information." << std::endl;
            std::cout << "--udp <ip_address> <port>         - set output device to UDP socket" << std::endl;

            return false;
        }
        else if (isOption(arg, "udp"))
        {
            if (optionalParam(_targetIp, argv, idx, argc))
            {
                ++idx;
                int port;
                if (optionalParam(port, argv, idx, argc))
                {
                    _targetPort = (unsigned)port;
                    ++idx;
                }
            }
        } else {
            _files.push_back(arg);
        }

    }
    return true;
}