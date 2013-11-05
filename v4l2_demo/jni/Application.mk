# Build both ARMv5TE and ARMv7-A machine code.
APP_PLATFORM = android-8

APP_ABI	:= armeabi-v7a
#APP_ABI := $(ARM_ARCH)

#Sam modify it to release
APP_OPTIM := release
#APP_OPTIM := debug
#APP_OPTIM = $(MY_OPTIM)

APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti

#sam modify it from gnustl_static to gnustl_shared
#APP_STL := gnustl_static
#APP_STL	:= gnustl_shared
APP_STL := gnustl_shared

#APP_CPPFLAGS += -fno-rtti


#
APP_CPPFLAGS += -Dlinux -fsigned-char
APP_CFLAGS += -fsigned-char
#APP_CPPFLAGS += $(MY_CPPFLAGS) -Dlinux
#STLPORT_FORCE_REBUILD := true

