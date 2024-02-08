// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once
#include <string>
#include<arpa/inet.h>
#include<sys/socket.h>

class Socket {
    int sock;
    sockaddr_in destination;

public:
    Socket();
    Socket(const std::string& host, unsigned int port);
    ~Socket();
    void connect(const std::string& host, unsigned int port);
    int send(const uint8_t* data, size_t len);
};