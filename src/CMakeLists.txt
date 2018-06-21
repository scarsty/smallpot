project(tinypot)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_COMPILER $ENV{CXX})
set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -pthread")

include_directories(../include . ../common)
link_directories(. ../lib)

file(GLOB SRC_LIST
    ./*.cpp
    ./tinyxml2/*.cpp
    ./hash/*.cpp
    ../common/libconvert.cpp
    ../common/File.cpp)

set(LINK_LIBRARIES iconv SDL2 SDL2_ttf SDL2_image SDL2main avutil avcodec avformat swresample swscale ass)

add_executable(tinypot ${SRC_LIST})
target_link_libraries(tinypot ${LINK_LIBRARIES})
