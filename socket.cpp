// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <string.h>
#include <unistd.h>
#include <exception>
#include "socket.h"

/// @brief Creates a new data packet with the given buffer and buffer length.
/// @param buffer
/// @param length
Packet::Packet(uint8_t *buffer, size_t length)
	: buff(buffer), buff_len(length)
{
}

/// @brief Gets the port of the source of the data
/// @return
uint16_t Packet::port()
{
	return ntohs(source.sin_port);
}

/// @brief Gets the address of the source of the data
/// @return
std::string Packet::addr()
{
	return std::string(inet_ntoa(source.sin_addr)); // copy
}

/// @brief Creates a new socket.
/// @note  call listen or connect to receive or send respectively.
Socket::Socket()
	: sock(-1)
{
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1)
	{
		throw std::exception();
	}
}

Socket::~Socket()
{
	::close(sock);
}

void Socket::listen(int port, in_addr_t addr)
{
	memset((char *)&local, 0, sizeof(local));

	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(addr);

	// bind socket to port
	if (::bind(sock, (struct sockaddr *)&local, sizeof(local)) == -1)
	{
		throw std::exception();
	}
}

/// @brief Blocking receive of a data packet
/// @param packet
/// @return
ssize_t Socket::receive(Packet &packet)
{
	socklen_t slen = sizeof(packet.source);
	packet.bytes_rec = recvfrom(sock, packet.buff, packet.buff_len, 0, (struct sockaddr *)&packet.source, &slen);
	return packet.bytes_rec;
}

void Socket::connect(const std::string &host, unsigned int port)
{
	memset((char *)&local, 0, sizeof(local));
	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = inet_addr(host.c_str());
}

int Socket::send(const uint8_t *data, size_t len)
{
	int nBytes = ::sendto(sock, data, len, 0, reinterpret_cast<sockaddr *>(&destination), sizeof(destination));
	return nBytes;
}