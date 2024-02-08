
#include "lua.h"



#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>



/////////////////////////////////////////////////////////////////////////
// Module
/////////////////////////////////////////////////////////////////////////


// Start the table that will hold the values for a module
Lua::Module::Module(Lua* lua, const char* name)
    : lua(lua)
    , name(name)
{
    lua_newtable(lua->L);
}

// Finish the table and register as global
Lua::Module::~Module()
{
    lua_setglobal(lua->L, name);
}

// Add a function to the current module table.
void Lua::Module::add(const char* name, lua_CFunction fn)
{
    lua_pushstring(lua->L, name);
    lua_pushcfunction(lua->L, fn);
    lua_settable(lua->L, -3);
}

// Add an integer to the current module table.
// Intended for constants / enums (which will be mutable)
void Lua::Module::add(const char* name, int value)
{
    lua_pushstring(lua->L, name);
    lua_pushinteger(lua->L, value);
    lua_settable(lua->L, -3);
}

/////////////////////////////////////////////////////////////////////////
// Core Lua
/////////////////////////////////////////////////////////////////////////


Lua::Lua(Simulation* simulation)
    : L(0)
    , simulation(simulation)

{
    L = luaL_newstate();
    *(Simulation**)lua_getextraspace(L) = simulation;

    // Save a pointer to this in the lua registry.
    // Use with Lua::get() to get pointer back.
    lua_pushlightuserdata(L, (void*)this);
    lua_setfield(L, LUA_REGISTRYINDEX, "TGLUAKEY");

    luaL_openlibs(L); // standard libraries

    // TODO - register methods
}

Lua::~Lua()
{
    if (L) {
       lua_close(L);
    }
}

// Starts the definition of a module with the given name
Lua::Module Lua::startModule(const char* name)
{
    return Module(this, name);
}

// Gets a pointer to this Lua object from the Lua registry.
Lua* Lua::get(lua_State* L) {
    int type = lua_getfield(L, LUA_REGISTRYINDEX, "TGLUAKEY");
    assert(type == LUA_TLIGHTUSERDATA);
    Lua* lua = (Lua*) lua_touserdata(L, -1);
    lua_pop(L, 1);
    return lua;
}

// Sets up a C method as a global with the given name.
void Lua::registerMethod(const char* name, lua_CFunction fn)
{
    lua_pushcfunction(L, fn);
    lua_setglobal(L, name);
}

// Runs the given set of lines as a chunk
int Lua::run(char** lines, std::string& errorMessage)
{
    while (*lines) {
        int error = luaL_loadstring(L, *lines) || lua_pcall(L, 0, 0, 0);
        if (error) {
            size_t len;
            const char* errmsg = lua_tolstring(L, -1, &len);
            errorMessage = std::string(errmsg, len);
            std::cout << errorMessage << std::endl;
            lua_pop(L, 1); // remove error message
            return 1;
        }

        ++lines;
    }
    return 0;
}

// Helper function for retrieving an error message
static void handleError(lua_State* L, std::string& errorMessage) {
    size_t len;
    const char* errmsg = lua_tolstring(L, -1, &len);
    errorMessage = std::string(errmsg, len);
    std::cout << errorMessage << std::endl;
    lua_pop(L, 1); // remove error message
}

// Loads and runs a file.
int Lua::runFile(const std::string& fileName, std::string& errorMessage)
{
    int error = luaL_loadfile(L, fileName.c_str()) || lua_pcall(L, 0, 0, 0);
    if (error) {
        handleError(L, errorMessage);
        return 1;
    }
    return 0;
}

// Loads a file
int Lua::loadFile(const std::string& fileName, std::string& errorMessage)
{
    int error = luaL_loadfile(L, fileName.c_str());
    if (error) {
        handleError(L, errorMessage);
        return 1;
    }
    return 0;
}

// Runs a previously loaded chunk.
int Lua::run(std::string& errorMessage)
{
    int error = lua_pcall(L, 0, 0, 0);
    if (error) {
        handleError(L, errorMessage);
        return 1;
    }
    return 0;
}
