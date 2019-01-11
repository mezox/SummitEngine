cmake_minimum_required(VERSION 3.7)
project(Application)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -stdlib=libc++")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(APPLICATION_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/include/AppDelegate.h
)

set(APPLICATION_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/src/main.mm
   ${CMAKE_CURRENT_SOURCE_DIR}/src/AppDelegate.mm
)

set(APPLICATION_RESOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/Main.storyboard
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/LaunchScreen.storyboard
)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
	${APPLICATION_HEADERS}
	${APPLICATION_SRCS}
	${APPLICATION_RESOURCES})

target_link_libraries(Application "-framework UIKit" Logging Engine)

set_target_properties(Application PROPERTIES
   MACOSX_BUNDLE TRUE
   RESOURCE "${APPLICATION_RESOURCES}"
   MACOSX_BUNDLE_GUI_IDENTIFIER "com.summit.engine"
   MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist
   XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist
)
