cmake_minimum_required(VERSION 3.0)

project(recovery)

set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS "`pkg-config --cflags --libs opencv`")

include_directories(./src ./lib )
include_directories(/usr/local/include/criu)

aux_source_directory(./src ./lib DIR_SRCS)

add_executable(recovery ${DIR_SRCS})

target_link_libraries(recovery libcriu.so rt)
target_link_libraries(recovery wiringPi)
