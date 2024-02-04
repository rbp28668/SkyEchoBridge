cmake_minimum_required(VERSION 3.25)
project(SkyEcho2 LANGUAGES C CXX VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


configure_file(version.h.in version.h)

add_executable(SkyEcho2 
    socket.cpp
    transport.cpp 
    message_handler.cpp
    fcs.cpp
    state.cpp
    NMEAData.cpp
    target.cpp
    ownship.cpp
    tracked_target.cpp
    outbound_flarm_converter.cpp
    main.cpp 
)

target_include_directories(SkyEcho2 PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

