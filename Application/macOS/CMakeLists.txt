cmake_minimum_required(VERSION 3.7)
project(Application)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -stdlib=libc++")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(APPLICATION_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/include/AppDelegate.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/SummitView.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/SummitWindow.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/ViewController.h
)

set(APPLICATION_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/src/main.m
   ${CMAKE_CURRENT_SOURCE_DIR}/src/SummitView.mm
   ${CMAKE_CURRENT_SOURCE_DIR}/src/SummitWindow.mm
   ${CMAKE_CURRENT_SOURCE_DIR}/src/AppDelegate.mm
   ${CMAKE_CURRENT_SOURCE_DIR}/src/ViewController.m
)

set(APPLICATION_RESOURCES
)

add_executable(Application MACOSX_BUNDLE
	${APPLICATION_HEADERS}
	${APPLICATION_SRCS}
	${APPLICATION_RESOURCES})

target_link_libraries(Application "-framework AppKit" Logging Engine)

set_target_properties(Application PROPERTIES
   MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist
)

set_source_files_properties(${APPLICATION_RESOURCES} PROPERTIES
   MACOSX_PACKAGE_LOCATION "Resources"
)
