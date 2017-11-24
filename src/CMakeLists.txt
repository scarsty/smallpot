project(tinypot)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_COMPILER $ENV{CXX})
set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -pthread")

include_directories(../include . ./others )
link_directories(. ../lib)

aux_source_directory(./ SRC_LIST)
aux_source_directory(./hash OTHER_SRC_LIST1)
aux_source_directory(./tinyxml2 OTHER_SRC_LIST2)
list(APPEND SRC_LIST ${OTHER_SRC_LIST1} ${OTHER_SRC_LIST2})

set(LINK_LIBRARIES SDL2 SDL2_ttf SDL2_image SDL2main avutil avcodec avformat swresample ass)

add_executable(tinypot ${SRC_LIST})
target_link_libraries(tinypot ${LINK_LIBRARIES})
