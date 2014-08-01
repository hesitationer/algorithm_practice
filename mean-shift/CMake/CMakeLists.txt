CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_CXX_FLAGS "--analyze")
SET(CMAKE_CXX_FLAGS "-Wall  -std=c++11")

SET(CMAKE_BUILD_TYPE "Debug")

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include"
	"/usr/local/include"
	"/home/eric/work-eric/ndk_proj/v4l2_demo/v4l2_demo/jni/include")

LINK_DIRECTORIES("~/Documents/opencv_lib_build_by_eric/Release/lib")

#################################################################
#my implementation
FILE(GLOB SRC_FILES
	"../src/mean_shift.cpp"
	"../src/ms_seg.cpp"
	"../test/pyrMSseg_case.cpp"
	"../test/test_case.cpp"
	"../test/test_main.cpp"
	"../../unit_test_utility/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(mean-shift ${SRC_FILES})
TARGET_LINK_LIBRARIES(mean-shift opencv_highgui opencv_imgproc opencv_core)

##############################################################
#opencv sample 
FILE(GLOB SRC_FILES
	"../src/camshiftdemo.cpp"
	"/home/eric/work-eric/ndk_proj/v4l2_demo/v4l2_demo/jni/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(camshift_demo ${SRC_FILES})
TARGET_LINK_LIBRARIES(camshift_demo opencv_highgui opencv_imgproc opencv_core opencv_video)

FILE(GLOB SRC_FILES  "../test/meanshift_segmentation.cpp")
ADD_EXECUTABLE(mean_shift_segmentation  ${SRC_FILES})
TARGET_LINK_LIBRARIES(mean_shift_segmentation opencv_highgui opencv_imgproc opencv_core opencv_video)
