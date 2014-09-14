LOCAL_PATH := $(call my-dir)

#
# PNG Library
# 
LOCAL_PATH := ../support/libpng-1.6.3
include $(CLEAR_VARS)

LOCAL_MODULE    := libpng
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
include $(BUILD_STATIC_LIBRARY)

#
# ZIP Library
# 
LOCAL_PATH := ../support/libzip-0.11.1
include $(CLEAR_VARS)

LOCAL_MODULE    := libzip
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
include $(BUILD_STATIC_LIBRARY)

#
# Bullet Physics Static Library
# 
LOCAL_PATH := ../support/bullet-2.82-r2704
include $(CLEAR_VARS)

LOCAL_MODULE    := libbullet
FILE_LIST := $(wildcard $(LOCAL_PATH)/BulletCollision/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/BulletDynamics/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/LinearMath/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
include $(BUILD_STATIC_LIBRARY)

#
# Open4Speed by L.Vonasek engine
# 
include $(CLEAR_VARS)
LOCAL_PATH := .

LOCAL_MODULE := open4speed

LOCAL_CFLAGS := -DANDROID_NDK

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
$(LOCAL_PATH)/../support/libpng-1.6.3 \
$(LOCAL_PATH)/../support/libzip-0.11.1 \
$(LOCAL_PATH)/../support/bullet-2.82-r2704 \
$(LOCAL_PATH)/../support/glm-0.9.5.4

LOCAL_SRC_FILES := \
open4speed.cpp \
car.cpp \
common.cpp \
physics/bullet.cpp \
input/keyboard.cpp \
input/airacer.cpp \
interfaces/model.cpp \
renderers/opengl/gles20.cpp \
renderers/opengl/glsl.cpp \
renderers/opengl/gltexture.cpp \
renderers/opengl/glfbo.cpp \
renderers/opengl/glvbo.cpp \
loaders/modelo4s.cpp \
loaders/pngloader.cpp \
loaders/rgb.cpp \
utils/switch.cpp \
utils/math.cpp \
utils/io.cpp
LOCAL_LDLIBS := -lGLESv2 -ldl -llog -landroid -lz

LOCAL_STATIC_LIBRARIES := libpng \ libbullet \ libzip

include $(BUILD_SHARED_LIBRARY)
