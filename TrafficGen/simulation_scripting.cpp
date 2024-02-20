#include <iostream>
#include <assert.h>
#include "simulation_scripting.h"
#include "simulation.h"

/////////////////////////////////////////////////////////////
// Diagnostics
#ifndef NDEBUG
static void _stackDump(lua_State *L, const char *label, std::ostream &os)
{
    os << "Stack at " << label << " ";
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++)
    { /* repeat for each level */
        int t = lua_type(L, i);
        switch (t)
        {

        case LUA_TSTRING: /* strings */
            os << lua_tostring(L, i);
            break;

        case LUA_TBOOLEAN: /* booleans */
            os << lua_toboolean(L, i) ? "true" : "false";
            break;

        case LUA_TNUMBER: /* numbers */
            os << lua_tonumber(L, i);
            break;

        default: /* other values */
            os << lua_typename(L, t);
            break;
        }
        os << "  "; /* put a separator */
    }

    os << std::endl
       << std::endl; /* end the listing */
}

static void _showTable(lua_State *L, const char *name, int t, std::ostream &os, int indent = 0)
{

    std::string prefix = std::string("").append(indent, ' ');

    if (name != nullptr)
    {
        std::cout << prefix << "Table " << name << std::endl;
    }

    if (lua_getmetatable(L, t))
    {
        std::cout << prefix << "Metatable:" << std::endl;
        _showTable(L, nullptr, -1, os, indent + 8);
        std::cout << prefix << "End Metatable" << std::endl;
        lua_pop(L, 1);
    }

    // table is in the stack at index 't'.
    t = lua_absindex(L, t); // Make sure relative ones work.

    bool isEmpty = true;
    lua_pushnil(L); /* first key */
    while (lua_next(L, t) != 0)
    {
        isEmpty = false;
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        const char *keyType = lua_typename(L, lua_type(L, -2));
        const char *valueType = lua_typename(L, lua_type(L, -1));

        switch (lua_type(L, -2))
        {
        case LUA_TSTRING: /* strings */
            os << prefix << lua_tostring(L, -2);
            break;

        case LUA_TNUMBER: /* numbers */
            os << prefix << lua_tonumber(L, -2);
            break;

        default:
            os << prefix << keyType;
            break;
        }

        os << "-->";

        switch (lua_type(L, -1))
        {

        case LUA_TSTRING: /* strings */
            os << lua_tostring(L, -1);
            break;

        case LUA_TBOOLEAN: /* booleans */
            os << lua_toboolean(L, -1) ? "true" : "false";
            break;

        case LUA_TNUMBER: /* numbers */
            os << lua_tonumber(L, -1);
            break;

        case LUA_TTABLE: // table
            os << "Table" << std::endl;
            _showTable(L, nullptr, -1, os, indent + 8);
            break;

        default: /* other values */
            os << valueType;
            break;
        }
        os << std::endl;

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }
    if (isEmpty)
    {
        os << prefix << "**EMPTY**" << std::endl;
    }
    os << std::endl;
}

// Make callable by Lua
int luaShowtable(lua_State *L)
{
    if (lua_istable(L, 1))
    {
        _showTable(L, "Table", 1, std::cout, 0);
    }
    else
    {
        luaL_argerror(L, 1, "Object isn't a table");
    }
    return 0;
}
#define stackDump(L, label, os) _stackDump(L, label, os)
#define showTable(L, name, index, os) _showTable(L, name, index, os)
#else
#define stackDump(L, label, os)
#define showTable(L, name, index, os)
#endif

/////////////////////////////////////////////////////////////

typedef struct ConstantT
{
    const char *name;
    int value;
} ConstantT;

// Address type constants.   Global AddressType
const static ConstantT AddressTypes[] = {
    {"ADSB_IACO", 0},          // ADS-B with ICAO address
    {"ADSB_SELF_ASSIGNED", 1}, // ADS-B with Self-assigned address
    {"TISB_ICAO", 2},          // TIS-B with ICAO address
    {"TISB_TRACK_FILE", 3},    // TIS-B with track file ID.
    {"SURFACE_VEHICLE", 4},    // Surface Vehicle
    {"GROUND_STATION", 5},     // Ground Station Beacon
    {nullptr, 0}};

// Emitter - see 3.5.1.10 EMITTER CATEGORY
const static ConstantT EmitterCategory[] = {
    {"UNKNOWN", 0},             // 0 - No aircraft type information
    {"LIGHT", 1},               // 1 - Light (ICAO) < 15 500 lbs
    {"SMALL", 2},               // 2 - Small - 15 500 to 75 000 lbs
    {"LARGE", 3},               // 3 - Large - 75 000 to 300 000 lbs
    {"LARGE_HV", 4},            // 4 - High Vortex Large (e.g., aircraft such as B757)
    {"HEAVY", 5},               // 5 - Heavy (ICAO) - > 300 000 lbs
    {"HIGHLY_MANEUVERABLE", 6}, // 6 - Highly Maneuverable > 5G acceleration and high speed
    {"ROTORCRAFT", 7},          // 7 - Rotorcraft
    {"GLIDER", 9},              // 9 - Glider/sailplane
    {"LTA", 10},                // 10 - Lighter than air
    {"PARACHUTIST", 11},        // 11 - Parachutist/sky diver
    {"ULTRALIGHT", 12},         // 12 - Ultra light/hang glider/paraglider
    {"UAV", 14},                // 14 - Unmanned aerial vehicle
    {"SPACE", 15},              // 15 - Space/transatmospheric vehicle
    {"EMERGENCY_VEHICLE", 17},  // 17 - Surface vehicle — emergency vehicle
    {"SERVICE_VEHICLE", 18},    // 18 - Surface vehicle — service vehicle
    {"POINT_OBSTACLE", 19},     // 19 - Point Obstacle (includes tethered balloons)
    {"CLUSTER_OBSTACLE", 20},   // 20 - Cluster Obstacle
    {"LINE_OBSTACLE", 21},      // 21 - Line Obstacle
    {nullptr, 0}};

const static ConstantT EmergencyTypes[] = {
    {"NONE", 0},                  // no emergency
    {"GENERAL", 1},               // general emergency
    {"MEDICAL", 2},               // medical emergency
    {"FUEL", 3},                  // minimum fuel
    {"COMMUNICATIONS", 4},        // no communication
    {"UNLAWFUL_INTERFERENCE", 5}, // unlawful interference
    {"DOWNED", 6},                // downed aircraft
    {nullptr, 0}};

static void registerConstants(lua_State *L, const char *name, const ConstantT *constants)
{
    lua_newtable(L); // T
    while (constants->name != nullptr)
    {
        lua_pushinteger(L, constants->value); // T,V
        lua_setfield(L, -2, constants->name);
        ++constants;
    }
    lua_setglobal(L, name);
}




// Methods for the derived object. Note these are closures
// and it is up to the method to retrieve the userdata
// traffic object from the first upvalue.
const luaL_Reg SimulationScripting::traffic_methods[] = {
    {"alert", SimulationScripting::isAlert},
    {"addressType", SimulationScripting::getAddressType},
    {"address", SimulationScripting::getAddress},
    {"latitude", SimulationScripting::getLatitude},
    {"longitude", SimulationScripting::getLongitude},
    {"altFeet", SimulationScripting::getAltFeet},
    {"miscIndicators", SimulationScripting::getMiscIndicators},
    {"nic", SimulationScripting::getNic},
    {"nacp", SimulationScripting::getNacp},
    {"emitter", SimulationScripting::getEmitter},
    {"speedKts", SimulationScripting::getSpeedKts},
    {"verticalVelocity", SimulationScripting::getVerticalVelocity},
    {"track", SimulationScripting::getTrack},
    {"callsign", SimulationScripting::getCallsign},
    {"emergency", SimulationScripting::getEmergency},

    // Setters
    {"setAlert", SimulationScripting::setAlert},
    {"setAddressType", SimulationScripting::setAddressType},
    {"setAddress", SimulationScripting::setAddress},
    {"setLatitude", SimulationScripting::setLatitude},
    {"setLongitude", SimulationScripting::setLongitude},
    {"setAltFeet", SimulationScripting::setAltFeet},
    {"setMiscIndicators", SimulationScripting::setMiscIndicators},
    {"setNic", SimulationScripting::setNic},
    {"setNacp", SimulationScripting::setNacp},
    {"setNacp", SimulationScripting::setNacp},
    {"setEmitter", SimulationScripting::setEmitter},
    {"setSpeedKts", SimulationScripting::setSpeedKts},
    {"setVerticalVelocity", SimulationScripting::setVerticalVelocity},
    {"setTrack", SimulationScripting::setTrack},
    {"setCallsign", SimulationScripting::setCallsign},
    {"setEmergency", SimulationScripting::setEmergency},

    {"getLocation", SimulationScripting::getLocation},
    {"move", SimulationScripting::move},
    {"makeConflictingTo", SimulationScripting::makeConflictingTo},
    {"distanceTo", SimulationScripting::distanceTo},
    {nullptr, nullptr}};

//////////////////////////////////////////////////////////////////////
// GETTERS

int SimulationScripting::isAlert(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushboolean(L, traffic->trafficAlert);
    return 1;
}
int SimulationScripting::getAddressType(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->addressType);
    return 1;
}
int SimulationScripting::getAddress(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->address);
    return 1;
}

int SimulationScripting::getLatitude(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushnumber(L, traffic->latitude);
    return 1;
}
int SimulationScripting::getLongitude(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushnumber(L, traffic->longitude);
    return 1;
}
int SimulationScripting::getAltFeet(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushnumber(L, traffic->altFeet);
    return 1;
}
int SimulationScripting::getMiscIndicators(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->miscIndicators);
    return 1;
}
int SimulationScripting::getNic(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->nic);
    return 1;
}
int SimulationScripting::getNacp(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->nacp);
    return 1;
}

int SimulationScripting::getEmitter(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->emitter);
    return 1;
}

int SimulationScripting::getSpeedKts(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushnumber(L, traffic->speedKts);
    return 1;
}

int SimulationScripting::getVerticalVelocity(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushnumber(L, traffic->verticalVelocity);
    return 1;
}

int SimulationScripting::getTrack(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushnumber(L, traffic->track);
    return 1;
}

int SimulationScripting::getCallsign(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushstring(L, std::string(traffic->callsign, 8).c_str());
    return 1;
}

int SimulationScripting::getEmergency(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    lua_pushinteger(L, traffic->emergency);
    return 1;
}
/////////////////////////////////////////////////////
// Setters

int SimulationScripting::setAlert(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->trafficAlert = lua_toboolean(L, 2);
    return 0;
}

int SimulationScripting::setAddressType(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    int at = luaL_checkinteger(L, 2);
    luaL_argcheck(L, at >= 0 && at <= 5, 2, "addressType must be in range 0..5 inclusive");
    traffic->addressType = at;
    return 0;
}

int SimulationScripting::setAddress(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->address = luaL_checkinteger(L, 2) & 0xFFFFFF;
    return 0;
}
int SimulationScripting::setLatitude(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->latitude = luaL_checknumber(L, 2);
    return 0;
}
int SimulationScripting::setLongitude(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->longitude = luaL_checknumber(L, 2);
    return 0;
}
int SimulationScripting::setAltFeet(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->altFeet = luaL_checknumber(L, 2);
    return 0;
}
int SimulationScripting::setMiscIndicators(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->miscIndicators = luaL_checkinteger(L, 2) & 0x0F;
    return 0;
}
int SimulationScripting::setNic(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->nic = luaL_checkinteger(L, 2) & 0x0F;
    return 0;
}
int SimulationScripting::setNacp(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->nacp = luaL_checkinteger(L, 2) & 0x0F;
    return 0;
}
int SimulationScripting::setEmitter(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    int e = luaL_checkinteger(L, 2);
    luaL_argcheck(L, e >= 0 && e <= 21, 2, "emitter must be in range 0..21 inclusive");
    traffic->emitter = e;
    return 0;
}
int SimulationScripting::setSpeedKts(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->speedKts = luaL_checknumber(L, 2);
    return 0;
}
int SimulationScripting::setVerticalVelocity(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->verticalVelocity = luaL_checknumber(L, 2);
    return 0;
}
int SimulationScripting::setTrack(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    traffic->track = luaL_checknumber(L, 2);
    return 0;
}
int SimulationScripting::setCallsign(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    std::string cs(luaL_checkstring(L, 2));
    cs.append("        ");
    cs = cs.substr(0, 8);
    traffic->setCallsign(cs);
    return 0;
}
int SimulationScripting::setEmergency(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    int e = luaL_checkinteger(L, 2);
    luaL_argcheck(L, e >= 0 && e <= 6, 2, "emergency must be in range 0..6 inclusive");
    traffic->emergency = e;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// MISC TRAFFIC
int SimulationScripting::getLocation(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    double lat = traffic->latitude;
    double lon = traffic->longitude;
    lua_pushnumber(L, lat);
    lua_pushnumber(L, lon);
    return 2;
}

// move( north, east)
int SimulationScripting::move(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    double north = luaL_checknumber(L, 1);
    double east = luaL_checknumber(L, 2);
    traffic->move(north, east);
    return 0;
}

int SimulationScripting::makeConflictingTo(lua_State *L)
{
    Traffic *traffic = getTraffic(L);
    Traffic *other = *(Traffic **)luaL_checkudata(L, 2, "TrafficGen.Target");
    double targetRangeNM = luaL_checknumber(L, 3);
    double relativeBearing = luaL_checknumber(L, 4);
    traffic->makeConflictingTo(other, targetRangeNM, relativeBearing);
    return 0;
}

int SimulationScripting::distanceTo(lua_State *L){
    Traffic *traffic = getTraffic(L);
    Traffic *other = *(Traffic **)luaL_checkudata(L, 2, "TrafficGen.Target");
    double dist = traffic->distanceTo(other);
    lua_pushnumber(L, dist);
    return 1;
  
}
////////////////////////////////////////////////////////////////////////////////////////////
// SIMULATION

// Create userdata and metatable
int SimulationScripting::createTarget(lua_State *L)
{
    Simulation *simulation = *(Simulation **)lua_getextraspace(L);
    Traffic *target = simulation->newTarget();
    return wrapTarget(L, target);
}

/// @brief Create a target based on an existing one.
/// Lua Param is the source target.
/// @param L
/// @return New target.
int SimulationScripting::createFromTarget(lua_State *L)
{
    Simulation *simulation = *(Simulation **)lua_getextraspace(L);
    TargetMessage *other = getTraffic(L); // first parameter
    Traffic *target = simulation->createFrom(other);
    return wrapTarget(L, target);
}

// Wraps the ownship with userdata and sets it as a global.
void SimulationScripting::addOwnship(Lua &lua, Ownship *ownship)
{
    lua_State *L = lua.State();
    //std::cout << "Ownship " << ownship->latitude << " " << ownship->longitude << " " << std::string(ownship->callsign, 8) << std::endl;
    wrapTarget(L, ownship);
    assert(lua_isuserdata(L, -1)); // confirm TOS is a userdata object.
    lua_setglobal(L, "ownship");
}

// Converts the target message to a wrapped traffic or ownship.
int SimulationScripting::wrapTarget(lua_State *L, TargetMessage *target)
{
    // Create a userdatum big enough to hold the pointer to the traffic
    size_t nbytes = sizeof(TargetMessage *);
    TargetMessage **tgt = (TargetMessage **)lua_newuserdata(L, nbytes); // UD
    *tgt = target;                                                      // save the pointer

    // Now, magic up the metatable for the userdata.
    // Will create if not already there, otherwise not.
    if (luaL_newmetatable(L, "TrafficGen.Target"))
    {
        // Set the metatable as its own __index value
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");

        luaL_setfuncs(L, traffic_methods, 0);
    }
    // One way or another, TOS has the metatable for this.
    // UD, meta
    lua_setmetatable(L, -2); // UD, meta  (pops a table from the stack and sets it as the metatable of the object at the given index.)
    return 1;                // Should be just the userdata now.
}

int SimulationScripting::tick(lua_State *L)
{
    Simulation *simulation = *(Simulation **)lua_getextraspace(L);
    simulation->tick();
    simulation->wait();
    return 0;
}

// https://www.lua.org/pil/28.2.html

void SimulationScripting::registerMethods(Lua &lua)
{
    Lua::Module module = lua.startModule("simulation");
    module.add("tick", SimulationScripting::tick);
    module.add("createTarget", SimulationScripting::createTarget);
    module.add("createTargetFrom", SimulationScripting::createFromTarget);
    module.add("showTable", luaShowtable);

    lua_State *L = lua.State();
    registerConstants(L, "Address", AddressTypes);
    registerConstants(L, "Emitter", EmitterCategory);
    registerConstants(L, "Emergency", EmergencyTypes);

    // The value 0xFFF is reserved to convey that no horizontal velocity information
    // is available.
    lua_pushinteger(L, 0xFFF);
    lua_setglobal(L, "SPEED_UNKNOWN");

    // The value 0x800 is reserved to convey that no vertical velocity information is
    // available.
    lua_pushinteger(L, 0x800);
    lua_setglobal(L, "VERTICAL_SPEED_UNKNOWN");
}