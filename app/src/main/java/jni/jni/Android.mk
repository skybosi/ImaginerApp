#获取当前目录
LOCAL_PATH := $(call my-dir)

#清除一些变量
include $(CLEAR_VARS)
LOCAL_MODULE := libdpcer
LOCAL_SRC_FILES := ../libs3/$(TARGET_ARCH_ABI)/libdpcer.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
#链接的第三方库
LOCAL_STATIC_LIBRARIES  := libdpcer
#要生成的库名
LOCAL_MODULE    := libImaginer
#需要引用的库
LOCAL_LDFLAGS  := -fPIC  -shared  -ldl -llog
LOCAL_CPPFLAGS += -fexceptions
#编译参数
LOCAL_CFLAGS := -Wall -O3 -enable-threads
#定义宏
LOCAL_C_INCLUDES :=$(LOCAL_PATH)/../include
#库对应的源文件
MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/../src/*.cpp)
LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)
#生成共享库
include $(BUILD_SHARED_LIBRARY)
#$(warning "the value of HOST_OS is$(HOST_OS)") 
ifeq ($(HOST_OS),windows)
$(cmd /k xcopy ..\libs\* ..\..\..\jniLibs  /Y /S)
else
$(shell cp -arf $(LOCAL_PATH)/../libs/* $(LOCAL_PATH)/../../../jniLibs)
endif