cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")

project(crawler C)

file(GLOB SOURCES src/*.c src/*/*.c)

find_package(Threads)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} curl ${CMAKE_THREAD_LIBS_INIT})
