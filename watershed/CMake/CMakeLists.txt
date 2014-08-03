CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_CXX_FLAGS "--analyze")
SET(CMAKE_CXX_FLAGS "-Wall  -std=c++11")

SET(CMAKE_BUILD_TYPE "Release")

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include"
	"/usr/include"
	"/usr/local/include"
	"../../unit_test_utility/include")

LINK_DIRECTORIES("~/Documents/opencv_lib_build_by_eric/Release/lib")

#################################################################
#my implementation
FILE(GLOB SRC_FILES
	"../src/my_watershed.cpp"
	"../test/test_case.cpp"
	"../../unit_test_utility/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(my_watershed ${SRC_FILES})
TARGET_LINK_LIBRARIES(my_watershed opencv_highgui opencv_imgproc opencv_core)

##############################################################
#opencv sample 
FILE(GLOB SRC_FILES "../test/watershed_demo.cpp")
ADD_EXECUTABLE(cv_demo ${SRC_FILES})
TARGET_LINK_LIBRARIES(cv_demo opencv_highgui opencv_imgproc opencv_core opencv_video)
