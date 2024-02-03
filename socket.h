#pragma once

#include <string>
#include<arpa/inet.h>
#include<sys/socket.h>

class Socket;

class Packet {
    struct sockaddr_in source;
    int source_port;
    size_t buff_len;
    uint8_t* buff;
    ssize_t bytes_rec;

    friend class Socket;

    public:
    Packet(uint8_t* buffer, size_t length);
    uint16_t port();
    std::string addr();
    uint8_t* buffer() { return buff;}
    size_t length() { return buff_len;}
    ssize_t received() { return bytes_rec;}
};

class Socket {

    struct sockaddr_in local;
    int handle;

    public:
    Socket(int port, in_addr_t addr = INADDR_ANY);
    ~Socket();
    ssize_t receive(Packet& packet);
};
