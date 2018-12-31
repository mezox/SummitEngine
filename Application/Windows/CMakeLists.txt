cmake_minimum_required(VERSION 3.8)
project(Application)

add_executable(Application main.cpp)
target_link_libraries(Application Logging Engine)
