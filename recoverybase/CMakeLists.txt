cmake_minimum_required(VERSION 3.0)

project(recovery)

set(CMAKE_CXX_STANDARD 17)

include_directories(./src 
                    ./lib
                    /usr/local/include/criu
                    )

aux_source_directory(./src DIR_SRC)


add_executable(recovery ${DIR_SRC})

target_link_libraries(recovery libcriu.so 
                                rt
                                )