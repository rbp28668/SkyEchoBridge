
#include <iostream>
#include <iomanip>
#include <unistd.h> 
#include "simulation.h"

static void showPacket(const uint8_t *buffer, size_t len)
{
    std::cout << std::setfill('0');
    for (int i = 0; i < len; ++i)
    {
        std::cout << (i > 0 ? "," : "") << std::hex << std::setw(2) << int(buffer[i]);
    }
    std::cout << std::dec << std::setfill(' ') << std::endl;
}


Simulation::Simulation(Socket *socket)
    : socket(socket)
{
    now = Clock::time();
}

Simulation::~Simulation(){
    for(auto t: _traffic){
        delete t;
    }
}

Traffic* Simulation::newTarget(){
    Traffic* t = new Traffic();
    _traffic.push_back(t);
    return t;
}

void Simulation::wait()
{
    auto then = now;
    while (now == then)
    {
        now = Clock::time();
        usleep(10000);
    }
}

void Simulation::tick()
{

   std::cout << "Sending at " << now << std::endl;
    
    heartbeat.build();
    frame.wrap(&heartbeat);
    int bytes = socket->send(frame.data(), frame.length());
    std::cout << "Sent " << bytes << " for heartbeat ";
    showPacket(frame.data(), frame.length());

    // Ownship
    _ownship.build();
    frame.wrap(&_ownship);
    bytes = socket->send(frame.data(), frame.length());
    std::cout << "Sent " << bytes << " for ownship" << std::endl;
    // showPacket(frame.data(), frame.length());

    // targets
    for (auto t : _traffic)
    {
        t->build();
        frame.wrap(t);
        bytes = socket->send(frame.data(), frame.length());
        std::cout << "Sent " << bytes << " for traffic" << std::endl;
    }
}