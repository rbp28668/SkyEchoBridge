// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include <iomanip>
#include "version.h"
#include "socket.h"
#include "transport.h"
#include "message_handler.h"
#include "outbound_flarm_converter.h"
#include "state.h"
#include "config.h"

// Set up a default configuration.
static Config config;

// One big receive buffer
const size_t buffer_size = 16384;
static uint8_t buffer[buffer_size];

// send to cout for the time being
static OutboundFlarmConverter outbound(std::cout);

// Holds the application state.  Gets updated
// by the message handler. 
// Can tell you you're abou to die....
static State state(&outbound, &config);

static void showPacket(uint8_t* buffer, size_t len){
    std::cout << "Received " << len << " bytes" << std::endl;
    std::cout << std::setfill('0');
    for(int i=0; i< len; ++i){
        std::cout << (i>0 ? "," : "") << std::hex << std::setw(2) << int(buffer[i]);
    }
    std::cout << std::dec << std::setfill(' ') << std::endl;
}


int main(int argc, char* argv[]){
    std::cout << "SkyEcho2 Bridge Version " << VERSION_MAJOR << '.' << VERSION_MINOR << std::endl;

    Packet packet(buffer, buffer_size);

    Socket socket(4000);

    MessageHandler handler(&state);
    Transport transport(&handler);
    while(socket.receive(packet) != -1){
        //showPacket(packet.buffer(), packet.received());
        transport.process(packet.buffer(), packet.received());
    }
    return 0;
}