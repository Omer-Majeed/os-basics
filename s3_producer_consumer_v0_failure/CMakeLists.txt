cmake_minimum_required(VERSION 3.20)
project(s3_producer_consumer_v0_failure CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# try O2, O3, some optimizations should make main waiting for ever
add_compile_options(-O1)

add_executable(s3_producer_consumer_v0_failure main.cc)