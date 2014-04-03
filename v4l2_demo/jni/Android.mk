LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := eric_v4l2
#LOCAL_C_INCLUDES := 
LOCAL_SRC_FILES := v4l2_mock_android.cpp
#LOCAL_LDLIBS := 
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := test_v4l2_camera
LOCAL_C_INCLUDES := ./include
LOCAL_SRC_FILES := \
	test/test_main.cpp \
	test/test.cpp \
	test/tools_test.cpp \
	src/V4L2_Camera.cpp	\
	src/pick_up_test_case.cpp \
	src/tools.cpp
#LOCAL_LDLIBS := 
include $(BUILD_EXECUTABLE)
