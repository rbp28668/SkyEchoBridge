// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#pragma once

#include <cstdint>
#include <stddef.h>

class State;
class Target;

/// @brief The guts of the message decoding.
/// See https://www.foreflight.com/connect/spec/
/// and Garming GDL90 spec
class MessageHandler{

    enum class MessageType {
        Heartbeat = 0,
        Initialization = 2,
        UplinkData = 7,
        HeightAboveTerrain = 9,
        OwnshipReport = 10,
        OwnshipGeometricAltitude = 11,
        TrafficReport = 20,
        BasicReport = 30,
        LongReport = 31,
        IdMessage = 101
    };

    // State we want to update from the messages.
    State* state;

    bool isFcsValid(uint8_t* msgStart, size_t msgLength);
    void processTargetMessage(Target* target, uint8_t* msgStart, size_t msgLength);

    void handleHeartbeat(uint8_t* msgStart, size_t msgLength);
    void handleUplinkData(uint8_t* msgStart, size_t msgLength);
    void handleHeightAboveTerrain(uint8_t* msgStart, size_t msgLength);
    void handleOwnshipReport(uint8_t* msgStart, size_t msgLength);
    void handleOwnshipGeometricAltitude(uint8_t* msgStart, size_t msgLength);
    void handleTrafficReport(uint8_t* msgStart, size_t msgLength);
    void handleBasicReport(uint8_t* msgStart, size_t msgLength);
    void handleLongReport(uint8_t* msgStart, size_t msgLength);
    void handleForeFlightMessage(uint8_t* msgStart, size_t msgLength);

    public:
    MessageHandler(State* state);
    void onMessage(uint8_t* msgStart, size_t msgLength);
};