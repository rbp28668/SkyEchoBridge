cmake_minimum_required(VERSION 3.18)
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
    stream_adapter.cpp
    outbound_pipe.cpp
    outbound_pipe_stream_receiver.cpp
    serial_port.cpp
    serial_stream_receiver.cpp
    stdout_stream_receiver.cpp
    config.cpp
    main.cpp 
)

target_include_directories(SkyEcho2 PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

