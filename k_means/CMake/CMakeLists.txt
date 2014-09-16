CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
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

#LINK_DIRECTORIES("/home/eric/Downloads/opencv-2.4.9/eric_build/lib")
#LINK_DIRECTORIES("/home/eric/ATempTemp/build_opencv/debug01/lib")
LINK_DIRECTORIES("/home/eric/Documents/opencv_lib_build_by_eric/Debug/lib")

FILE(GLOB SRC_FILES
	"../src/K_means_cluster.cpp"
	"../test/test_case.cpp"
	"../../unit_test_utility/src/pick_up_test_case.cpp")
ADD_EXECUTABLE(kmeans ${SRC_FILES})
TARGET_LINK_LIBRARIES(kmeans opencv_highgui opencv_imgproc opencv_core)

FILE(GLOB SRC_FILES "../test/kmeans.cpp")
ADD_EXECUTABLE(cv_kmeans ${SRC_FILES})
TARGET_LINK_LIBRARIES(cv_kmeans opencv_highgui opencv_imgproc opencv_core)
