CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(socket_client)
SET(PROJECT_ROOT_PATH "../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/lib")

SET(CMAKE_CXX_FLAGS "-Wall -Werror")

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/include" "/usr/local/include" "/home/eric/JeenonTechnology/ObjDetectorFramework/AndroidFramework/inc")

LINK_DIRECTORIES("/usr/local/lib/")

FILE(GLOB  SRC_FILES "../src/socket_server.cpp" "../test/socket_server_test.cpp" "/home/eric/JeenonTechnology/ObjDetectorFramework/AndroidFramework/jni/net.cpp")
ADD_EXECUTABLE(am_server ${SRC_FILES})
TARGET_LINK_LIBRARIES(am_server pthread)

FILE(GLOB  SRC_FILES "../src/am_client.cpp" "../test/am_client_test.cpp" "/home/eric/JeenonTechnology/ObjDetectorFramework/AndroidFramework/jni/net.cpp")
ADD_EXECUTABLE(am_client ${SRC_FILES})
TARGET_LINK_LIBRARIES(am_client pthread)

