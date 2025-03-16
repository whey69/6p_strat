cmake_minimum_required(VERSION 3.10)
project(6p C)

include_directories(.)

add_executable(main main.c)
target_link_libraries(main PRIVATE raylib m)