#pragma once
#include "lua.h"
#include "target_message.h"

class SimulationScripting
{


	// Functions for Traffic object
	static const  luaL_Reg traffic_methods [];

public:
	static 	void registerMethods(Lua& lua);
	static void addOwnship(Lua& lua, Ownship* ownship);

private:

	inline static Traffic* getTraffic(lua_State* L) { return *(Traffic **) luaL_checkudata(L, 1, "TrafficGen.Target");}

	// Getters
	static int isAlert(lua_State *L);
	static int getAddressType(lua_State *L);
	static int getAddress(lua_State *L);
	static int getLatitude(lua_State *L);
	static int getLongitude(lua_State *L);
	static int getAltFeet(lua_State *L);
	static int getMiscIndicators(lua_State *L);
	static int getNic(lua_State *L);
	static int getNacp(lua_State *L);
	static int getEmitter(lua_State *L);
	static int getSpeedKts(lua_State *L);
	static int getVerticalVelocity(lua_State *L);
	static int getTrack(lua_State *L);
	static int getCallsign(lua_State *L);
	static int getEmergency(lua_State *L);

	// Setters
	static int setAlert(lua_State *L);
	static int setAddressType(lua_State *L);
	static int setAddress(lua_State *L);
	static int setLatitude(lua_State *L);
	static int setLongitude(lua_State *L);
	static int setAltFeet(lua_State *L);
	static int setMiscIndicators(lua_State *L);
	static int setNic(lua_State *L);
	static int setNacp(lua_State *L);
	static int setEmitter(lua_State *L);
	static int setSpeedKts(lua_State *L);
	static int setVerticalVelocity(lua_State *L);
	static int setTrack(lua_State *L);
	static int setCallsign(lua_State *L);
	static int setEmergency(lua_State *L);

	// Misc
	static int getLocation(lua_State *L);
	static int move(lua_State *L);

	static int wrapTarget(lua_State* L, TargetMessage* target);
	static int createTarget(lua_State* L);
	static int createLightTarget(lua_State* L);

	static int tick(lua_State* L);


};
