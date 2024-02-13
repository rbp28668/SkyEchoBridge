#include <iostream>
#include "simulation_scripting.h"
#include "simulation.h"

// https://www.lua.org/pil/28.2.html

void SimulationScripting::registerMethods(Lua &lua)
{
    Lua::Module module = lua.startModule("simulation");
    module.add("tick", SimulationScripting::tick);
    module.add("createTarget", SimulationScripting::createTarget);
}


/*  Address type constants.   Global AddressType
t = 0 : ADS-B with ICAO address
t = 1 : ADS-B with Self-assigned address
t = 2 : TIS-B with ICAO address
t = 3 : TIS-B with track file ID.
t = 4 : Surface Vehicle
t = 5 : Ground Station Beacon

The value 0xFFF is reserved to convey that no horizontal velocity information
is available.

The value 0x800 is reserved to convey that no vertical velocity information is
available.


 Emitter - see 3.5.1.10 EMITTER CATEGORY
 
p = 0 : no emergency
p = 1 : general emergency
p = 2 : medical emergency
p = 3 : minimum fuel
p = 4 : no communication
p = 5 : unlawful interference
p = 6 : downed aircraft
 
 */


// __index: If it is a function, it is called with table and key as arguments,
// and the result of the call (adjusted to one value) is the result of the operation.
int SimulationScripting::index(lua_State *L)
{
    std::cout << "index" << std::endl;

    void *ud = luaL_checkudata(L, 1, "TrafficGen.Target");
    if(ud) {
        Traffic* traffic = *(Traffic**)ud;
        const char* field = luaL_checkstring(L, 2);

        std::string f(field);

        if(f.compare("trafficAlert") == 0){
            lua_pushboolean(L, traffic->trafficAlert);
        } else if(f.compare("addressType") == 0){
            lua_pushinteger(L, traffic->addressType);
        } else if(f.compare("address") == 0){
            lua_pushinteger(L, traffic->address);
        } else if(f.compare("latitude") == 0){
            lua_pushnumber(L,traffic->latitude);
        } else if( f.compare("longitude") == 0) {
            lua_pushnumber(L, traffic->longitude);
        } else if(f.compare("altFeet") == 0){
            lua_pushnumber(L, traffic->altFeet);
        } else if(f.compare("miscIndicators") == 0){
            lua_pushinteger(L, traffic->miscIndicators);
        } else if(f.compare("nic") == 0){
           lua_pushinteger(L, traffic->nic);
        } else if(f.compare("nacp") == 0){
           lua_pushinteger(L, traffic->nacp);
        } else if(f.compare("emitter") == 0){
           lua_pushinteger(L, traffic->emitter);
        } else if(f.compare("speedKts") == 0){
            lua_pushnumber(L, traffic->speedKts);
        } else if(f.compare("verticalVelocity") == 0){
            lua_pushnumber(L, traffic->verticalVelocity);
        } else if(f.compare("track") == 0){
            lua_pushnumber(L, traffic->track);
        } else if(f.compare("callsign") == 0){
            lua_pushstring(L, std::string(traffic->callsign, 8).c_str());
        } else if(f.compare("emergency") == 0){
            lua_pushinteger(L, traffic->emergency);
        } else {
            luaL_argerror(L, 2, std::string("Unknown field ").append(f).c_str());
        }
    } else {
        luaL_argerror (L, 1, "Object isn't traffic");
    }
    
    return 1;
}

// If it is a function, it is called with table, key, and value as arguments.
int SimulationScripting::newIndex(lua_State *L)
{
    void *ud = luaL_checkudata(L, 1, "TrafficGen.Target");
    if(ud) {
        Traffic* traffic = *(Traffic**)ud;
        const char* field = luaL_checkstring(L, 2);

        std::string f(field);
        std::cout << "New Index Field: " << f << std::endl;
        if(f.compare("trafficAlert") == 0){
            traffic->trafficAlert = lua_toboolean(L, 3);
        } else if(f.compare("addressType") == 0){
            int at = luaL_checkinteger(L, 3);
            luaL_argcheck (L, at >= 0 && at <= 5, 3, "addressType must be in range 0..5 inclusive");
            traffic->addressType = at;
        } else if(f.compare("address") == 0){
            traffic->address = luaL_checkinteger(L,3) & 0xFFFFFF;
        }  else if(f.compare("latitude") == 0){
            traffic->latitude = luaL_checknumber(L, 3);
        } else if( f.compare("longitude") == 0) {
            traffic->longitude = luaL_checknumber(L, 3);
        } else if(f.compare("altFeet") == 0){
            traffic->altFeet = luaL_checknumber(L,3);
        } else if(f.compare("miscIndicators") == 0){
            traffic->miscIndicators = luaL_checkinteger(L, 3) & 0x0F;
        } else if(f.compare("nic") == 0){
           traffic->nic = luaL_checkinteger(L, 3) & 0x0F;
        } else if(f.compare("nacp") == 0){
           traffic->nacp = luaL_checkinteger(L, 3) & 0x0F;
        } else if(f.compare("emitter") == 0){
            int e = luaL_checkinteger(L, 3);
            luaL_argcheck (L, e >= 0 && e <= 21, 3, "emitter must be in range 0..21 inclusive");
            traffic->emitter = e;
        } else if(f.compare("speedKts") == 0){
            traffic->speedKts = luaL_checknumber(L, 3);
        } else if(f.compare("verticalVelocity") == 0){
            traffic->verticalVelocity = luaL_checknumber(L, 3);
        } else if(f.compare("track") == 0){
            traffic->track = luaL_checknumber(L,3);
        } else if(f.compare("callsign") == 0){
            std::string cs(luaL_checkstring(L,3));
            cs.append("        ");
            cs = cs.substr(0,8);
            traffic->setCallsign(cs);
        } else if(f.compare("emergency") == 0){
            int e = luaL_checkinteger(L,3);
            luaL_argcheck (L, e >= 0 && e <= 6, 3, "emergency must be in range 0..6 inclusive");
            traffic->emergency = e;
        } else {
            luaL_argerror(L, 2, std::string("Unknown field ").append(f).c_str());
        }
    } else {
        luaL_argerror (L, 1, "Object isn't traffic");
    }
    
    return 0;
}


// Create a table with its own metatable.  the index and newindex
// methods are set up as closures with the target address as parameers
// This means that only the C++ code can access that address.  Each
// table has its own metatable as the upvalues change.
int SimulationScripting::createTarget(lua_State *L)
{
    Simulation *simulation = *(Simulation **)lua_getextraspace(L);
    Traffic *target = simulation->newTarget();

    // Create a userdatum big enough to hold the pointer to the traffic
    size_t nbytes = sizeof(Traffic*);
    Traffic** tgt = (Traffic **)lua_newuserdata(L, nbytes);     // MT, UD
    *tgt = target;  // save the pointer

    // Now, magic up the metatable for the userdata.
    // Will create if not already there, otherwise not.
    if(luaL_newmetatable(L, "TrafficGen.Target")){
        // - MT
        
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, SimulationScripting::index);       // MT, __index, cf
        lua_settable(L, -3);  /* metatable.__index = index */   // MT

        lua_pushstring(L, "__newindex");                        // MT, __newindex
        lua_pushcfunction(L, SimulationScripting::newIndex);    // MT, __newindex, cf
        lua_settable(L, -3);                                    // MT
    }
    // One way or another, TOS has the metatable for this.

    // pops a table from the stack and sets it as the metatable of the object at the given index.
    lua_setmetatable(L, -2);
    
     return 1; // Should be just the userdatum now.
}


int SimulationScripting::tick(lua_State *L)
{
    Simulation *simulation = *(Simulation **)lua_getextraspace(L);
    simulation->tick();
    simulation->wait();
    return 0;
}
