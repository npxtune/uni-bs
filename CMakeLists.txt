cmake_minimum_required(VERSION 3.26)
project(uni-bs)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(uni-bs source/main.c source/init.c source/data_storage.c)