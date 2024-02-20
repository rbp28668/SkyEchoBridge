cmake_minimum_required(VERSION 3.25)
project(TrafficGen LANGUAGES C CXX VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


configure_file(version.h.in version.h)

add_executable(TrafficGen 
    frame.cpp
    heartbeat_message.cpp 
    target_message.cpp
    fcs.cpp
    socket.cpp
    lua.cpp
    simulation.cpp
    simulation_scripting.cpp
    main.cpp 
)

target_link_libraries(TrafficGen  liblua.a)
target_link_libraries(TrafficGen -L./Lua/lua-5.4.6/)

target_include_directories(TrafficGen PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

