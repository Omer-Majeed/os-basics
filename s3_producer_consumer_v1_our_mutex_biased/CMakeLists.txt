cmake_minimum_required(VERSION 3.20)
project(s3_producer_consumer_v1_our_mutex_biased CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-O2)

add_executable(s3_producer_consumer_v1_our_mutex_biased main.cc)