cmake_minimum_required(VERSION 3.15)
project(minicontainer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SRC_FILES
    src/main.cpp
    src/core/lifecycle.cpp
    src/storage/overlay.cpp
    src/core/cgroup.cpp
    src/core/network.cpp     
)

add_executable(minicontainer ${SRC_FILES}
    src/core/cgroup.cpp
    include/container_config.h)