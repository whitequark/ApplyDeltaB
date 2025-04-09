cmake_minimum_required(VERSION 3.15)
project(ApplyDeltaB C)

add_executable(ApplyDeltaB main.c)
set_property(TARGET ApplyDeltaB PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_link_libraries(ApplyDeltaB msdelta)
