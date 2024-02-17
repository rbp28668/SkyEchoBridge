#pragma once
#include "lua.h"


class SimulationScripting
{

	// Functions for userdata metatable
	static const  luaL_Reg traffic_m [];
	// Functions for derived object
	static const  luaL_Reg traffic_o [];

public:
	static 	void registerMethods(Lua& lua);
private:
	static int createTarget(lua_State* L);
	static int tick(lua_State* L);

    // For target metatable
	static int index(lua_State* L);
    static int newIndex(lua_State* L);
};
