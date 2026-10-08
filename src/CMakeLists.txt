cmake_minimum_required(VERSION 3.20)

if(DEFINED ENV{CXX})
    set(CMAKE_CXX_COMPILER $ENV{CXX})
endif()

project(smallpot LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_LIST
    ./*.cpp
    ./others/*.cpp
    ../mlcc/strfunc.cpp
    ../mlcc/filefunc.cpp
    ../mlcc/Engine.cpp)

add_executable(smallpot ${SRC_LIST})

target_include_directories(smallpot PRIVATE
    .
    ../include
    ../mlcc
    /usr/local/include)

target_link_directories(smallpot PRIVATE
    .
    ../lib
    ../lib/x64
    /usr/local/lib)

if(NOT MSVC)
    target_compile_options(smallpot PRIVATE -O3 -pthread)
    target_link_options(smallpot PRIVATE -pthread)
endif()

target_link_libraries(smallpot PRIVATE
    iconv
    SDL3
    SDL3_ttf
    SDL3_image
    avutil
    avcodec
    avformat
    swresample
    swscale
    ass)
