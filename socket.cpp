// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <string.h>
#include <unistd.h>
#include <exception>
#include "socket.h"

/// @brief Creates a new data packet with the given buffer and buffer length.
/// @param buffer 
/// @param length 
Packet::Packet(uint8_t* buffer, size_t length)
: buff(buffer)
, buff_len(length)
{
    
}

/// @brief Gets the port of the source of the data
/// @return 
uint16_t Packet::port(){
    return ntohs(source.sin_port);
}

/// @brief Gets teh address of the source of the data
/// @return 
std::string Packet::addr(){
    return std::string(inet_ntoa(source.sin_addr)); // copy
}


/// @brief Creates a new socket to listen on the given port
/// @param port 
/// @param addr 
Socket::Socket(int port, in_addr_t addr)
: handle(-1)
{
    handle =socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(handle == -1)
	{
		throw std::exception();
	}

    memset((char *) &local, 0, sizeof(local));
	
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(addr);
	
	//bind socket to port
	if( bind(handle , (struct sockaddr*)&local, sizeof(local) ) == -1)
	{
		throw std::exception();
	}
}

Socket::~Socket(){
    ::close(handle);
}

/// @brief Blocking receive of a data packet
/// @param packet 
/// @return 
ssize_t Socket::receive(Packet& packet) {
    socklen_t slen = sizeof(packet.source);
    packet.bytes_rec = recvfrom(handle, packet.buff, packet.buff_len, 0, (struct sockaddr *) &packet.source, &slen);
    return packet.bytes_rec;
}