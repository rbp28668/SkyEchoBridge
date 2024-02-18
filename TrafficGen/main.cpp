// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include <iomanip>

#include "version.h"
#include "socket.h"
#include "simulation.h"
#include "ownship_scripting.h"
#include "simulation_scripting.h"


int main(int argc, char* argv[]){
    std::cout << "TrafficGen Version " << VERSION_MAJOR << '.' << VERSION_MINOR << std::endl;

    Socket socket("127.0.0.1", 4000);

 
    Simulation simulation(&socket);
  
    simulation.ownship()->address = 0x405603;
    simulation.ownship()->setCallsign("GLIDR952");
    simulation.ownship()->emitter = 9; // glider
    simulation.ownship()->latitude = 52;
    simulation.ownship()->longitude = -0.5;
    simulation.ownship()->nic = 10;
    simulation.ownship()->nacp = 10;
    simulation.ownship()->altFeet = 3000;
    simulation.ownship()->speedKts = 70;
    simulation.ownship()->track = 195;
    simulation.ownship()->miscIndicators = 0x81;  // airborn and true track.

    Lua lua(&simulation);
    //OwnShipScripting::registerMethods(lua);
    SimulationScripting::registerMethods(lua);
    SimulationScripting::addOwnship(lua, simulation.ownship());

    for(int i=1; i<argc; ++i){
        std::string file(argv[i]);
        std::string err("Unable to load and run LUA file ");
        err.append(file);
        lua.runFile(file, err);

    }

    // while(true){
    //     simulation.wait();
    //     simulation.tick();
    // }
    return 0;
}