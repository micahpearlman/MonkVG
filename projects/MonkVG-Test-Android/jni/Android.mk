# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH       := $(call my-dir)
top_srcdir := $(call my-dir)/../../..
c_includes := \
		$(top_srcdir)/glu/include $(top_srcdir)/include \
		$(top_srcdir)/src $(top_srcdir)/src/opengl \
		$(top_srcdir)/thirdparty/gles2-bc/Sources/OpenGLES \
		$(top_srcdir)/thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES11 \
		$(top_srcdir)/thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES20 \
		$(top_srcdir)/thirdparty/fmemopen $(LOCAL_PATH)/boost/include

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenVG
LOCAL_SRC_FILES := ../../MonkVG-Android/obj/local/armeabi/libOpenVG.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenVGU
LOCAL_SRC_FILES := ../../MonkVG-Android/obj/local/armeabi/libOpenVGU.a
LOCAL_STATIC_LIBRARIES := libOpenVG
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE     := libmonkvg
LOCAL_CFLAGS     := $(cflags)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_LDLIBS     := -llog -lGLESv1_CM -lGLESv2
LOCAL_SRC_FILES  := gl_code.cpp
LOCAL_STATIC_LIBRARIES := libOpenVGU libOpenVG
include $(BUILD_SHARED_LIBRARY)


