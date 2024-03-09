// SkyEchoBridge
// Copyright R Bruce Porteous 2024

#pragma once
#include <string>
#include <vector>
class Config {

public:

private:

    // Params for output via UDP
    std::string _targetIp;
    uint _targetPort;

    std::vector<std::string> _files;

    bool isOption(const std::string& arg, const std::string& option);
    bool requiredParam(std::string &param, char *argv[], int idx, int argc, const char *err);
    bool requiredParam(int &param, char *argv[], int idx, int argc, const char *err);
    bool optionalParam(std::string &param, char *argv[], int idx, int argc);
    bool optionalParam(int &param, char *argv[], int idx, int argc);


    public:

    Config();

    bool update(int argc, char* argv[]);

   
    // IP address to send Socket UDP output to.
    const std::string& targetIp() const {return  _targetIp;}
    // Port to send Socket UDP output to.
    unsigned int targetPort() const {return _targetPort;}

    // Anything that's not an option.
    std::vector<std::string>& files() {return _files;}

};