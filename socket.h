// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <string>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "stream_receiver.h"

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

/// @brief A UDP socket capable of reading and writing
/// @note Implements StreamReceiver so a connected socket can have
/// an ostream wrapped round it via StreamAdapter and thus
/// allow characters to be written easily into UDP packets
/// which are sent when flush() is called.
class Socket : public StreamReceiver {

    struct sockaddr_in local;
    struct sockaddr_in destination;

    int sock;

    public:
    Socket();
    ~Socket();

    void listen(int port, in_addr_t addr = INADDR_ANY);
    void connect(const std::string& host, unsigned int port);
    ssize_t receive(Packet& packet);
    int send(const uint8_t* data, size_t len);

    virtual bool receive(const char* data, size_t length) { return send(reinterpret_cast<const uint8_t*>(data), length) == length;}
};
