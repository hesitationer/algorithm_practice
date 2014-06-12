CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_CXX_FLAGS "--analyze")
SET(CMAKE_CXX_FLAGS "-Wall  -std=c++11")

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include"
	"/usr/local/include"
	"/home/eric/work-eric/ndk_proj/v4l2_demo/v4l2_demo/jni/include")

LINK_DIRECTORIES("/usr/local/lib/")

FILE(GLOB SRC_FILES
	"../src/mean_shift.cpp"
	"../test/*.cpp"
	"/home/eric/work-eric/ndk_proj/v4l2_demo/v4l2_demo/jni/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(mean-shift ${SRC_FILES})
TARGET_LINK_LIBRARIES(mean-shift opencv_highgui opencv_imgproc opencv_core)

FILE(GLOB SRC_FILES
	"../src/camshiftdemo.cpp"
	"/home/eric/work-eric/ndk_proj/v4l2_demo/v4l2_demo/jni/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(camshift_demo ${SRC_FILES})
TARGET_LINK_LIBRARIES(camshift_demo opencv_highgui opencv_imgproc opencv_core opencv_video)
