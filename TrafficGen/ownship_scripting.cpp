#include "ownship_scripting.h"
#include "simulation.h"

void OwnShipScripting::registerMethods(Lua& lua)
{
    Lua::Module module = lua.startModule("ownship");
    module.add("setLocation", OwnShipScripting::setLocation);
    module.add("setHeight", OwnShipScripting::setHeight);
    module.add("setTrack", OwnShipScripting::setTrack);
    module.add("setSpeed", OwnShipScripting::setSpeed);
    module.add("setClimbRate", OwnShipScripting::setClimbRate);
    module.add("getAltitude", OwnShipScripting::getAltitude);
    module.add("getTrack", OwnShipScripting::getTrack);
    module.add("getLocation", OwnShipScripting::getLocation);
}
     
int OwnShipScripting::setLocation(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double lat = luaL_checknumber(L, 1);
    double lon = luaL_checknumber(L, 2);
    simulation->ownship().latitude = lat;
    simulation->ownship().longitude = lon;
    return 0;
}
int OwnShipScripting::setHeight(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double height = luaL_checknumber(L, 1);
    simulation->ownship().altFeet = height;
    return 0;
}
int OwnShipScripting::setTrack(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double track = luaL_checknumber(L, 1);
    simulation->ownship().track = track;
    return 0;
}
int OwnShipScripting::setSpeed(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double speed = luaL_checknumber(L, 1);
    simulation->ownship().speedKts = speed;
    return 0;
}
int OwnShipScripting::setClimbRate(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double climb = luaL_checknumber(L, 1);
    simulation->ownship().descentFPM = -climb;
    return 0;
}
int OwnShipScripting::getAltitude(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double altitude = simulation->ownship().altFeet;
    lua_pushnumber(L, altitude);
    return 1;
}
int OwnShipScripting::getTrack(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double track = simulation->ownship().track;
    lua_pushnumber(L, track);
    return 1;
}
int OwnShipScripting::getLocation(lua_State* L){
    Simulation* simulation = *(Simulation**)lua_getextraspace(L);
    double lat = simulation->ownship().latitude;
    double lon = simulation->ownship().longitude;
    lua_pushnumber(L, lat);
    lua_pushnumber(L, lon);
    return 2;
}

