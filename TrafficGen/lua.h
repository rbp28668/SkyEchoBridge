#pragma once

#include <string>
#include <set>
#include <map>


extern "C" {
#include "Lua/lua-5.4.6/src/lua.h"
#include "Lua/lua-5.4.6/src/lauxlib.h"
#include "Lua/lua-5.4.6/src/lualib.h"
}

class Simulation;

class Lua
{


	// Simulator this implementation is tied to.
	Simulation* simulation;

	// Core Lua state.
	lua_State* L;
	


public:
	
	/// <summary>
	/// Helper class to create Lua modules with C functions and constant enums.
	/// </summary>
	class Module {
		Lua* lua;
		const char* name;
	
	public:
		Module(Lua* lua, const char* name);
		~Module();
		void add(const char* name, lua_CFunction fn);
		void add(const char* name, int value);
	};



	Simulation* getSimulation() { return simulation; }

	Lua(Simulation* simulation);
	~Lua();

	Module startModule(const char* name);

	// Retrieve this instance from the lua registry.
	static Lua* get(lua_State* L);

	void registerMethod(const char* name, lua_CFunction);
	int run(char** lines, std::string& errorMessage);
	int runFile(const std::string& fileName, std::string& errorMessage);

	int loadFile(const std::string& fileName, std::string& errorMessage);
	int run(std::string& errorMessage);


};
