cmake_minimum_required(VERSION 3.15)
project(advent C)

set(CMAKE_C_STANDARD 99)

add_executable(day01 day01/main.c)
add_executable(day02 day02/main.c)
add_executable(day03 day03/main.c)
add_executable(day04 day04/main.c)
add_executable(day05 day05/main.c)
target_link_libraries(day05 m)