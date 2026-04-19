cmake_minimum_required(VERSION 4.0)
project(SpaceGame C)

set(CMAKE_C_STANDARD 23)

add_executable(SpaceGame main.c)
target_link_libraries(SpaceGame -lmingw32 -lalleg44)
