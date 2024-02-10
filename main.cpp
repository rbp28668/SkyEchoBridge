// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include <iomanip>
#include "version.h"
#include "socket.h"
#include "transport.h"
#include "message_handler.h"
#include "outbound_flarm_converter.h"
#include "stream_adapter.h"
#include "state.h"
#include "config.h"
#include "fcs.h"

// Set up a default configuration.
static Config config;

// One big receive buffer
const size_t buffer_size = 16384;
static uint8_t buffer[buffer_size];



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

    #ifndef NDEBUG
    std::cout << "DEBUG BUILD" << std::endl;
    FCS::validateTable();
    #endif

    // Outbound socket to send to XCSoar
    Socket outboundSocket;
    outboundSocket.connect("192.168.0.95",4353);

    // Allow it to be used as an ostream.
    OutputStreamAdapter osa(&outboundSocket);

    #if 1
    // And tell the FLARM convert to use it.
    OutboundFlarmConverter outbound(osa);
    #else
       OutboundFlarmConverter outbound(std::cout);
    #endif
    // Holds the application state.  Gets updated
    // by the message handler. 
    // Can tell you you're abou to die....
    State state(&outbound, &config);
    Packet packet(buffer, buffer_size);

    Socket socket;
    socket.listen(4000);

    MessageHandler handler(&state);
    Transport transport(&handler);
    while(socket.receive(packet) != -1){
        //showPacket(packet.buffer(), packet.received());
        transport.process(packet.buffer(), packet.received());
    }
    return 0;
}