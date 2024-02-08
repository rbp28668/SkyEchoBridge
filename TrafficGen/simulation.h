#pragma once
#include <vector>
#include "socket.h"
#include "frame.h"
#include "clock.h"
#include "heartbeat_message.h"
#include "target_message.h"

class Simulation {
    Ownship _ownship;
    std::vector<Traffic*> _traffic;
   
    Frame frame;
   
    uint32_t now;

    HeartbeatMessage heartbeat;

    Socket* socket;

public:
    Simulation(Socket* socket);
    ~Simulation();

    Ownship& ownship() { return _ownship;}
    Traffic* newTarget();

    void wait();
    void tick();
};

