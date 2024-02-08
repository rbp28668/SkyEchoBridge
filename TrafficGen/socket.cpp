// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <unistd.h> // close
#include <iostream>
#include "socket.h"

    Socket::Socket()
    : sock(-1)
    {

    }

    Socket::Socket(const std::string& host, unsigned int port)
    : sock(-1)
    {
        connect(host, port);
    }
    
    Socket::~Socket(){
        if(sock != -1) ::close(sock);
    }
    
    void Socket::connect(const std::string& host, unsigned int port){
    
        sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        std::cout << "sock: " << sock << std::endl;
        destination.sin_family = AF_INET;
        destination.sin_port = htons(port);
        destination.sin_addr.s_addr = inet_addr(host.c_str());
        destination.sin_addr.s_addr = 0x0100007F; // BODGE localhost
     }

    int Socket::send(const uint8_t* data, size_t len){
        int nBytes = ::sendto(sock, data, len, 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
        return nBytes;
    }

