CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_CXX_FLAGS "--analyze")
SET(CMAKE_CXX_FLAGS "-Wall  -std=c++11")

SET(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include"
	"/usr/local/include"
	"../../unit_test_utility/include")

LINK_DIRECTORIES("/usr/local/lib/")

FILE(GLOB SRC_FILES
	"../src/*.cpp"
	"../test/*.cpp"
	"../../unit_test_utility/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(find_corners ${SRC_FILES})
TARGET_LINK_LIBRARIES(find_corners opencv_highgui opencv_imgproc opencv_core)
