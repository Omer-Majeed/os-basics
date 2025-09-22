cmake_minimum_required(VERSION 3.20)
project(s0_thread_race CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(s0_thread_race race.cc)