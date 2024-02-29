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
#include "stdout_stream_receiver.h"
#include "serial_stream_receiver.h"
#include "outbound_pipe_stream_receiver.h"
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

    // Magic up some form of output stream receiver
    StreamReceiver* psr = nullptr;
    switch(config.outputType()){
        case Config::OutputType::Console:
            psr = new StdoutStreamReceiver();
        break;

        case Config::OutputType::Pipe:
            psr = new OutboundPipeStreamReceiver(config.fifo().c_str());
        break;

        case Config::OutputType::Serial:
            psr = new SerialStreamReceiver(config.serialDevice().c_str(), config.baudRate());
        break;

        case Config::OutputType::Socket:
            // Outbound socket to send to XCSoar
            Socket* outboundSocket = new Socket();
            outboundSocket->connect(config.targetIp(),config.targetPort());
            psr = outboundSocket;
            break;
    }


    // Allow it to be used as an ostream.
    OutputStreamAdapter osa(psr);

    // And tell the FLARM convert to use it.
    OutboundFlarmConverter outbound(osa);

    // Holds the application state.  Gets updated
    // by the message handler. 
    // Can tell you you're abou to die....
    State state(&outbound, &config);
    Packet packet(buffer, buffer_size);

    Socket socket;
    socket.listen(config.listenPort());

    MessageHandler handler(&state);
    Transport transport(&handler);
    while(socket.receive(packet) != -1){
        //showPacket(packet.buffer(), packet.received());
        transport.process(packet.buffer(), packet.received());
    }

    if(psr) delete psr;
    
    return 0;
}