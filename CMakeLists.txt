cmake_minimum_required(VERSION 3.0)
project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdParty)

# Make test executable
add_executable(tests main.cpp)
target_link_libraries(tests doctest)