CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11")

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include"
	"/usr/local/include"
	"/home/eric/work_eric/ndk_proj/v4l2_demo/v4l2_demo/jni/include"
	"/home/eric/JeenonTechnology/ObjDetectorFramework/AndroidFramework/inc")

LINK_DIRECTORIES("/usr/local/lib/")

FILE(GLOB SRC_FILES
	"../src/*.cpp"
	"../test/test_main.cpp"
	"/home/eric/JeenonTechnology/ObjDetectorFramework/AndroidFramework/jni/net.cpp")
ADD_EXECUTABLE(server_ra ${SRC_FILES})
TARGET_LINK_LIBRARIES(server_ra pthread)

FILE(GLOB client_ra_src
	"../src/*.cpp"
	"../test/*.cpp"
	"/home/eric/JeenonTechnology/ObjDetectorFramework/AndroidFramework/jni/net.cpp"
	"/home/eric/work_eric/ndk_proj/v4l2_demo/v4l2_demo/jni/src/pick_up_test_case.cpp")
FILE(GLOB file_remove "../test/test_main.cpp")
list(REMOVE_ITEM client_ra_src ${file_remove})
ADD_EXECUTABLE(client_ra ${client_ra_src})
TARGET_LINK_LIBRARIES(client_ra pthread)

