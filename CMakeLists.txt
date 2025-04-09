cmake_minimum_required(VERSION 3.10)
project(ApplyDeltaB C)

add_executable(ApplyDeltaB main.c)
target_link_libraries(ApplyDeltaB msdelta)