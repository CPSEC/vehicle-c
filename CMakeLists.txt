cmake_minimum_required(VERSION 3.0)

project (test)

set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS -pthread)
#set(CMAKE_CXX_FLAGS "`pkg-config --cflags --libs opencv`")
#set(CMAKE_CXX_FLAGS "-I/usr/local/include -lraspicam -lmmal -lmmal_core -lmmal_util")


aux_source_directory(. DIR_SRCS)

#find_package(Threads REQUIRED)
#target_link_libraries(main${CMAKE_THREAD_LIBS_INIT})


set(CMAKE_MODULE_PATH "/usr/local/lib/cmake/${CMAKE_MODULE_PATH}") 
find_package(raspicam REQUIRED)
find_package(OpenCV)



add_executable(test ${DIR_SRCS})


target_link_libraries(test wiringPi)
target_link_libraries(test ${raspicam_CV_LIBS})


