cmake_minimum_required(VERSION 2.8.3)

project(3rdParty)

set(DOCTEST_WITH_TESTS OFF)
set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF)
set(DOCTEST_NO_INSTALL ON)

set(IDE_PROJECT_FOLDER "3rdParty")

add_subdirectory(imgui)
add_subdirectory(doctest)
add_subdirectory(json)
add_subdirectory(microprofile)
