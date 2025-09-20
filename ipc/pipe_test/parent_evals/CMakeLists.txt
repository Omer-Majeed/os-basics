cmake_minimum_required(VERSION 3.20)
project(parent_side_evals C)
set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard is cached for visibility in libraries." FORCE)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(parent_side_evals parent_eval.c)