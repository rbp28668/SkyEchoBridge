cmake_minimum_required(VERSION 3.18)
project(FlarmMerge LANGUAGES C CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


configure_file(version.h.in version.h)

add_executable(FlarmMerge 
    flarm_message.cpp
    flarm_receiver.cpp
    inbound_pipe.cpp
    ../serial_port.cpp
    inbound_pipe_epoll_adapter.cpp
    serial_epoll_adapter.cpp
    message_merge.cpp
    main.cpp 
)

target_include_directories(FlarmMerge PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

