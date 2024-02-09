// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include <iomanip>

#include "version.h"
#include "socket.h"
#include "simulation.h"



int main(int argc, char* argv[]){
    std::cout << "TrafficGen Version " << VERSION_MAJOR << '.' << VERSION_MINOR << std::endl;

    Socket socket("127.0.0.1", 4000);
 
    Simulation simulation(&socket);

    simulation.ownship().address = 0x405603;
    simulation.ownship().setCallsign("GLIDR952");
    simulation.ownship().emitter = 9; // glider
    simulation.ownship().latitude = 52;
    simulation.ownship().longitude = -0.5;
    simulation.ownship().nic = 10;
    simulation.ownship().nacp = 10;
    simulation.ownship().altFeet = 3000;
    simulation.ownship().speedKts = 70;
    simulation.ownship().track = 42;
    simulation.ownship().miscIndicators = 0x81;  // airborn and true track.


    Traffic* traffic = simulation.newTarget();
    traffic->address = 0x123456;
    traffic->setCallsign("GCKFY");
    traffic->emitter = 9; // glider
    traffic->latitude = 52;
    traffic->longitude = -0.499;
    traffic->nic = 10;
    traffic->nacp = 10;
    traffic->altFeet = 3000;
    traffic->speedKts = 70;
    traffic->track = 42;
    traffic->miscIndicators = 0x81;  // airborn and true track.

    while(true){
        simulation.wait();
        simulation.tick();
    }
    return 0;
}