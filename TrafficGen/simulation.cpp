
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
    , nextAddress(1)
{
    now = Clock::time();
}

Simulation::~Simulation(){
    for(auto t: _traffic){
        delete t;
    }
}

static std::string& toCallsign(std::string& cs, unsigned int number, int digits){
     if(digits == 0) return cs;
    unsigned int digit = number % 26;
    number = number / 26;
    return toCallsign(cs, number, digits-1).append(1,char('A' + digit));
}

Traffic* Simulation::newTarget(){
    Traffic* t = new Traffic();
    t->address = nextAddress;

    std::string cs = "GC";
    cs = toCallsign(cs,nextAddress-1,3);
    t->setCallsign(cs);

    ++nextAddress;
    _traffic.push_back(t);
    return t;
}

Traffic* Simulation::createFrom( TargetMessage* source){
    Traffic* traffic = newTarget();

    traffic->latitude = source->latitude;
    traffic->longitude = source->longitude;
    traffic->altFeet = source->altFeet;
    traffic->miscIndicators = source->miscIndicators;
    traffic->emitter = source->emitter; // What sort of thing this is - see 3.5.1.10 EMITTER CATEGORY
    traffic->speedKts = source->speedKts;   // Note 0xFFF or 4095 is unknown.
    traffic->verticalVelocity = source->verticalVelocity; // 0x800 is unknown.
    traffic->track = source->track;    // see miscIndicators for exactly what this means.

   // For simulation
     traffic->descentFPM = source->descentFPM;

    return traffic;
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

    //std::cout << "Sending at " << now << std::endl;
    
    heartbeat.build();
    frame.wrap(&heartbeat);
    int bytes = socket->send(frame.data(), frame.length());
    //std::cout << "Sent " << bytes << " for heartbeat ";
    //showPacket(frame.data(), frame.length());

    // Ownship
    _ownship.tick();
    _ownship.build();
    frame.wrap(&_ownship);
    bytes = socket->send(frame.data(), frame.length());
    //std::cout << "Sent " << bytes << " for ownship" << std::endl;
    // showPacket(frame.data(), frame.length());

    // targets
    for (auto t : _traffic)
    {
        t->tick();
        t->build();
        frame.wrap(t);
        bytes = socket->send(frame.data(), frame.length());
        //std::cout << "Sent " << bytes << " for traffic" << std::endl;
    }
}