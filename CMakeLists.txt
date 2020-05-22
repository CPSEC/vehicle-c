project (test)

set(CMAKE_CXX_FLAGS "-std=c++17")

aux_source_directory(. DIR_SRCS)

add_executable(test ${DIR_SRCS})