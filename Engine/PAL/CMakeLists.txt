cmake_minimum_required(VERSION 3.8)

project(PAL)

SET(TARGET_PROJECT_FOLDER "${TARGET_PROJECT_FOLDER}/PAL")

add_subdirectory(FileSystem)
add_subdirectory(RenderAPI)