cmake_minimum_required(VERSION 3.0)
project(producer_consumer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -fdiagnostics-color=auto -fmax-errors=2 -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fmax-errors=2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

set(SOURCE_FILES src/main.cpp)

add_executable(main ${SOURCE_FILES})

find_package(Threads)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
