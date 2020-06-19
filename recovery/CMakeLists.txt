cmake_minimum_required(VERSION 3.13)

project(recovery)

set(CMAKE_CXX_FLAGS "-std=c++17")

include_directories(./src /usr/local/include/criu)
aux_source_directory(./src DIR_SRCS)

add_executable(recovery ${DIR_SRCS})

target_link_libraries(recovery libcriu.so)
