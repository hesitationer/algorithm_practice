LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := Camera_V4L2
LOCAL_SRC_FILES := v4l2_util.cpp
LOCAL_CXXFLAGS := -D_GLIBCXX_USE_WCHAR_T
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := eric_v4l2#Test_V4L2
LOCAL_SRC_FILES := v4l2_mock_android.cpp#main.cpp
LOCAL_LDLIBS := -llog -L../libs/armeabi-v7a/ -lCamera_V4L2
include $(BUILD_EXECUTABLE)

