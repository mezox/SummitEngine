cmake_minimum_required(VERSION 3.6.0)

project(Summit)

set(TARGET_PROJECT_FOLDER "${TARGET_PROJECT_FOLDER}/Summit")

add_subdirectory(Core)
add_subdirectory(Math)
add_subdirectory(Logging)
add_subdirectory(PAL)
add_subdirectory(Renderer)
add_subdirectory(Engine)
