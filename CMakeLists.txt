cmake_minimum_required(VERSION 3.28)
project(DockerProject)

set(CMAKE_CXX_STANDARD 17)

add_executable(DockerProject server.c data_storage.h data_storage.c)
