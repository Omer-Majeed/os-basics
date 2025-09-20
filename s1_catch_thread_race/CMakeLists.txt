cmake_minimum_required(VERSION 3.20)
project(threads_lab CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# To catch thread race
add_compile_options(-fsanitize=thread)
add_link_options(-fsanitize=thread)

# This is used to catch memory leakages, out-of-bounds, use-after-free, double free, stack buffer overflow
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

add_executable(race race.cc)