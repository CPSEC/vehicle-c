project (test)

set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS -pthread)

aux_source_directory(. DIR_SRCS)

#find_package(Threads REQUIRED)
#target_link_libraries(main${CMAKE_THREAD_LIBS_INIT})

add_executable(test ${DIR_SRCS})
