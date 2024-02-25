cmake_minimum_required(VERSION 3.25)
project(FlarmMerge LANGUAGES C CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


configure_file(version.h.in version.h)

add_executable(FlarmMerge 
    flarm_message.cpp
    flarm_receiver.cpp
    inbound_pipe.cpp
    message_merge.cpp
    main.cpp 
)

target_include_directories(FlarmMerge PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

