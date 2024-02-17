#include <iostream>
#include "simulation_scripting.h"
#include "simulation.h"

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
    lua_setglobal(L,name);
}

// modified so the userdata is retrieved from the upvalue
// Effectively these functions are closures that have
// captured the base object.
static int wombles(lua_State *L)
{
    int idx = lua_upvalueindex(1);
    void *ud = luaL_checkudata(L, idx, "TrafficGen.Target");
    if (ud)
    {
        Traffic *traffic = *(Traffic **)ud;
        const char *field = luaL_checkstring(L, 1);
        std::cout << "Traffic " << std::string(traffic->callsign, 8) << " " << std::hex << traffic->address << std::dec << field << std::endl;
    }
    else
    {
        luaL_argerror(L, 1, "Object isn't traffic");
    }
    return 0;
}

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
    os << std::endl; /* end the listing */
}

#define stackDump(L, label, os) _stackDump(L, label, os)
#else
#define stackDump(L, label, os)
#endif

// Methods for the metatable.
const luaL_Reg SimulationScripting::traffic_m[] = {
    {"__index", SimulationScripting::index},
    {"__newindex", SimulationScripting::newIndex},
    {NULL, NULL}};

// Methods for the derived object. Note these are closures
// and it is up to the method to retrieve the userdata
// traffic object from the first upvalue.
const luaL_Reg SimulationScripting::traffic_o[] = {
    {"wombles", wombles},
    {NULL, NULL}};

// __index: If it is a function, it is called with table and key as arguments,
// and the result of the call (adjusted to one value) is the result of the operation.
int SimulationScripting::index(lua_State *L)
{
    // std::cout << "index" << std::endl;

    void *ud = luaL_checkudata(L, 1, "TrafficGen.Target");
    if (ud)
    {
        Traffic *traffic = *(Traffic **)ud;
        const char *field = luaL_checkstring(L, 2);

        std::string f(field);

        if (f.compare("trafficAlert") == 0)
        {
            lua_pushboolean(L, traffic->trafficAlert);
        }
        else if (f.compare("addressType") == 0)
        {
            lua_pushinteger(L, traffic->addressType);
        }
        else if (f.compare("address") == 0)
        {
            lua_pushinteger(L, traffic->address);
        }
        else if (f.compare("latitude") == 0)
        {
            lua_pushnumber(L, traffic->latitude);
        }
        else if (f.compare("longitude") == 0)
        {
            lua_pushnumber(L, traffic->longitude);
        }
        else if (f.compare("altFeet") == 0)
        {
            lua_pushnumber(L, traffic->altFeet);
        }
        else if (f.compare("miscIndicators") == 0)
        {
            lua_pushinteger(L, traffic->miscIndicators);
        }
        else if (f.compare("nic") == 0)
        {
            lua_pushinteger(L, traffic->nic);
        }
        else if (f.compare("nacp") == 0)
        {
            lua_pushinteger(L, traffic->nacp);
        }
        else if (f.compare("emitter") == 0)
        {
            lua_pushinteger(L, traffic->emitter);
        }
        else if (f.compare("speedKts") == 0)
        {
            lua_pushnumber(L, traffic->speedKts);
        }
        else if (f.compare("verticalVelocity") == 0)
        {
            lua_pushnumber(L, traffic->verticalVelocity);
        }
        else if (f.compare("track") == 0)
        {
            lua_pushnumber(L, traffic->track);
        }
        else if (f.compare("callsign") == 0)
        {
            lua_pushstring(L, std::string(traffic->callsign, 8).c_str());
        }
        else if (f.compare("emergency") == 0)
        {
            lua_pushinteger(L, traffic->emergency);
        }
        else
        {
            luaL_argerror(L, 2, std::string("Unknown field ").append(f).c_str());
        }
    }
    else
    {
        luaL_argerror(L, 1, "Object isn't traffic");
    }

    return 1;
}

// If it is a function, it is called with table, key, and value as arguments.
int SimulationScripting::newIndex(lua_State *L)
{
    void *ud = luaL_checkudata(L, 1, "TrafficGen.Target");
    if (ud)
    {
        Traffic *traffic = *(Traffic **)ud;
        const char *field = luaL_checkstring(L, 2);

        std::string f(field);
        // std::cout << "New Index Field: " << f << std::endl;
        if (f.compare("trafficAlert") == 0)
        {
            traffic->trafficAlert = lua_toboolean(L, 3);
        }
        else if (f.compare("addressType") == 0)
        {
            int at = luaL_checkinteger(L, 3);
            luaL_argcheck(L, at >= 0 && at <= 5, 3, "addressType must be in range 0..5 inclusive");
            traffic->addressType = at;
        }
        else if (f.compare("address") == 0)
        {
            traffic->address = luaL_checkinteger(L, 3) & 0xFFFFFF;
        }
        else if (f.compare("latitude") == 0)
        {
            traffic->latitude = luaL_checknumber(L, 3);
        }
        else if (f.compare("longitude") == 0)
        {
            traffic->longitude = luaL_checknumber(L, 3);
        }
        else if (f.compare("altFeet") == 0)
        {
            traffic->altFeet = luaL_checknumber(L, 3);
        }
        else if (f.compare("miscIndicators") == 0)
        {
            traffic->miscIndicators = luaL_checkinteger(L, 3) & 0x0F;
        }
        else if (f.compare("nic") == 0)
        {
            traffic->nic = luaL_checkinteger(L, 3) & 0x0F;
        }
        else if (f.compare("nacp") == 0)
        {
            traffic->nacp = luaL_checkinteger(L, 3) & 0x0F;
        }
        else if (f.compare("emitter") == 0)
        {
            int e = luaL_checkinteger(L, 3);
            luaL_argcheck(L, e >= 0 && e <= 21, 3, "emitter must be in range 0..21 inclusive");
            traffic->emitter = e;
        }
        else if (f.compare("speedKts") == 0)
        {
            traffic->speedKts = luaL_checknumber(L, 3);
        }
        else if (f.compare("verticalVelocity") == 0)
        {
            traffic->verticalVelocity = luaL_checknumber(L, 3);
        }
        else if (f.compare("track") == 0)
        {
            traffic->track = luaL_checknumber(L, 3);
        }
        else if (f.compare("callsign") == 0)
        {
            std::string cs(luaL_checkstring(L, 3));
            cs.append("        ");
            cs = cs.substr(0, 8);
            traffic->setCallsign(cs);
        }
        else if (f.compare("emergency") == 0)
        {
            int e = luaL_checkinteger(L, 3);
            luaL_argcheck(L, e >= 0 && e <= 6, 3, "emergency must be in range 0..6 inclusive");
            traffic->emergency = e;
        }
        else
        {
            luaL_argerror(L, 2, std::string("Unknown field ").append(f).c_str());
        }
    }
    else
    {
        luaL_argerror(L, 1, "Object isn't traffic");
    }

    return 0;
}

// Create userdata and metatable for a base class. Then derive from
// that so that we can add extra functions.
// Note that derived class methods are all closures that capture
// the userdata object as an upvalue.
int SimulationScripting::createTarget(lua_State *L)
{
    Simulation *simulation = *(Simulation **)lua_getextraspace(L);
    Traffic *target = simulation->newTarget();

    // Create a userdatum big enough to hold the pointer to the traffic
    size_t nbytes = sizeof(Traffic *);
    Traffic **tgt = (Traffic **)lua_newuserdata(L, nbytes); // UD
    *tgt = target;                                          // save the pointer

    // Now, magic up the metatable for the userdata.
    // Will create if not already there, otherwise not.
    if (luaL_newmetatable(L, "TrafficGen.Target"))
    {
        luaL_setfuncs(L, traffic_m, 0);
    }
    // One way or another, TOS has the metatable for this.
    // UD, meta

    // So set userdata metatable (but leave userdata & metatable on the stack)
    lua_pushvalue(L, -1);    // UD, meta, meta
    lua_setmetatable(L, -3); // UD, meta  (pops a table from the stack and sets it as the metatable of the object at the given index.)

    // Create a metatable (MT) for the derived object with just __index & __newindex
    // pointing to the metatable of the base userdata object. (Lua inheritance pattern)
    lua_createtable(L, 0, 2);                           // 2 entries, __index & __newindex
    lua_pushstring(L, "__index");                       // UD,meta, MT, _index
    lua_pushvalue(L, -3);                               // UD,meta, MT, _index, meta
    lua_settable(L, -3); /* metatable.__index = meta */ // UD,meta, MT

    lua_pushstring(L, "__newindex"); // UD,meta, MT, __newindex
    lua_pushvalue(L, -3);
    ;                    // UD,meta, MT, __newindex, meta
    lua_settable(L, -3); // UD,meta, MT

    //stackDump(L, "creating metatable: ", std::cout);

    // derived class table
    lua_newtable(L);         // UD, meta, MT, T
    lua_pushvalue(L, -2);    // UD, meta, MT, T, MT
    lua_setmetatable(L, -2); // UD, meta, MT, T

    //stackDump(L, "creating derived class: ", std::cout);

    // Tidy up stack a bit
    lua_replace(L, -3); // UD, T, MT
    lua_pop(L, 1);      // UD, T

    //stackDump(L, "After tidy: ", std::cout);

    // Now add functions to T - they use an upvalue to hold the userdata object (UD)
    // When nup is not zero, all functions are created with nup upvalues, initialized with copies of the nup values
    // previously pushed on the stack on top of the library table. These values are popped from the stack after the registration.
    lua_rotate(L, -2, 1); // T, UD (hopefully)

    //stackDump(L, "after rotate", std::cout);

    luaL_setfuncs(L, traffic_o, 1);

    //stackDump(L, "End", std::cout);

    return 1; // Should be just the table now.
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

    lua_State *L = lua.State();
    registerConstants(L, "Address", AddressTypes);
    registerConstants(L, "Emitter", EmitterCategory);
    registerConstants(L, "Emergency", EmergencyTypes);

    // The value 0xFFF is reserved to convey that no horizontal velocity information
    // is available.
    lua_pushinteger(L,0xFFF);
    lua_setglobal(L, "SPEED_UNKNOWN");

    // The value 0x800 is reserved to convey that no vertical velocity information is
    //available.
    lua_pushinteger(L,0x800);
    lua_setglobal(L, "VERTICAL_SPEED_UNKNOWN");


}