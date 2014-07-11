CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(optical_flow)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_CXX_FLAGS "--analyze")
SET(CMAKE_CXX_FLAGS "-Wall  -std=c++11")

SET(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include"
	"/usr/local/include"
	"../../unit_test_utility/include")

LINK_DIRECTORIES("/home/eric/Documents/opencv_lib_build_by_eric/Debug/lib")
#LINK_DIRECTORIES("/home/eric/Documents/opencv_lib_build_by_eric/Release/lib")

FILE(GLOB SRC_FILES
	"../src/*.cpp"
	"../test/test_case.cpp"
	"../test/test_main.cpp"
	"../../unit_test_utility/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(optical_flow ${SRC_FILES})
TARGET_LINK_LIBRARIES(optical_flow opencv_highgui opencv_imgproc opencv_core opencv_legacy)

# test case all in  one from Leo
ADD_EXECUTABLE(all_in_one "../opencv_sample/evalate_optical_flow_demo.cc")
TARGET_LINK_LIBRARIES(all_in_one opencv_highgui opencv_imgproc opencv_core
	opencv_legacy opencv_video)
