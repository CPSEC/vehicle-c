cmake_minimum_required(VERSION 3.0)

project(recovery)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "/usr/local/lib/cmake/${CMAKE_MODULE_PATH}") 

include_directories(./src 
                    ./lib
                    /usr/local/include/criu
                    ./lib/raspicam
                    )

aux_source_directory(./src DIR_SRC)
aux_source_directory(./lib DIR_LIB)

find_package(raspicam REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(recovery ${DIR_SRC} ${DIR_LIB})

target_link_libraries(recovery libcriu.so 
                                rt
                                wiringPi
                                ${raspicam_CV_LIBS}
                                )