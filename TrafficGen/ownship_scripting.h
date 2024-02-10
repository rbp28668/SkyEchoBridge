#pragma once
#include "lua.h"


class OwnShipScripting
{
public:
	static 	void registerMethods(Lua& lua);
private:
	static int setLocation(lua_State* L);
	static int setHeight(lua_State* L);
	static int setTrack(lua_State* L);
	static int setSpeed(lua_State* L);
	static int setClimbRate(lua_State* L);
	static int getAltitude(lua_State* L);
	static int getTrack(lua_State* L);
	static int getLocation(lua_State* L);

};