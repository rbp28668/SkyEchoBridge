// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include <iomanip>
#include <unistd.h> 
#include "version.h"
#include "socket.h"
#include "frame.h"
#include "clock.h"
#include "heartbeat_message.h"
#include "target_message.h"


static void showPacket(const uint8_t* buffer, size_t len){
    std::cout << std::setfill('0');
    for(int i=0; i< len; ++i){
        std::cout << (i>0 ? "," : "") << std::hex << std::setw(2) << int(buffer[i]);
    }
    std::cout << std::dec << std::setfill(' ') << std::endl;
}

int main(int argc, char* argv[]){
    std::cout << "TrafficGen Version " << VERSION_MAJOR << '.' << VERSION_MINOR << std::endl;

    Socket socket("localhost", 4000);
    Frame frame;
   
    auto now = Clock::time();

    HeartbeatMessage heartbeat;

    Ownship ownship;
    ownship.address = 0x405603;
    ownship.setCallsign("GLIDR952");
    ownship.emitter = 9; // glider
    ownship.latitude = 52;
    ownship.longitude = -0.5;
    ownship.nic = 10;
    ownship.nacp = 10;
    ownship.altFeet = 3000;
    ownship.speedKts = 70;
    ownship.track = 42;
    ownship.miscIndicators = 0x81;  // airborn and true track.


    Traffic traffic;
    traffic.address = 0x123456;
    traffic.setCallsign("GCKFY");
    traffic.emitter = 9; // glider
    traffic.latitude = 52;
    traffic.longitude = -0.499;
    traffic.nic = 10;
    traffic.nacp = 10;
    traffic.altFeet = 3000;
    traffic.speedKts = 70;
    traffic.track = 42;
    traffic.miscIndicators = 0x81;  // airborn and true track.

    while(true){
        
        auto then = now;
        while(now == then){
            now = Clock::time();
            usleep(10000);
        }
        
        std::cout << "Sending at " << now << std::endl;
    
        heartbeat.build();
        frame.wrap(&heartbeat);
        int bytes = socket.send(frame.data(), frame.length());
        std::cout << "Sent " << bytes << " for heartbeat ";
        showPacket(frame.data(), frame.length());
        
        // Ownship
        ownship.build();
        frame.wrap(&ownship);
        bytes = socket.send(frame.data(), frame.length());
        std::cout << "Sent " << bytes << " for ownship" << std::endl;
        //showPacket(frame.data(), frame.length());

        // targets
        traffic.build();
        frame.wrap(&traffic);
        bytes = socket.send(frame.data(), frame.length());
        std::cout << "Sent " << bytes << " for traffic" << std::endl;

    }
    return 0;
}