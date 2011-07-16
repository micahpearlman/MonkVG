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
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libmonkvg
LOCAL_CFLAGS    := -Werror
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../glu/include \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src \
    $(LOCAL_PATH)/../../../src/opengl
    
LOCAL_SRC_FILES := \
    gl_code.cpp \
    ../../../glu/libtess/dict.c \
    ../../../glu/libtess/geom.c \
    ../../../glu/libtess/memalloc.c \
    ../../../glu/libtess/mesh.c \
    ../../../glu/libtess/normal.c \
    ../../../glu/libtess/priorityq.c \
    ../../../glu/libtess/render.c \
    ../../../glu/libtess/sweep.c \
    ../../../glu/libtess/tess.c \
    ../../../glu/libtess/tessmono.c \
    ../../../glu/libutil/error.c \
    ../../../glu/libutil/glue.c \
    ../../../glu/libutil/project.c \
    ../../../glu/libutil/registry.c \
    ../../../src/opengl/glBatch.cpp \
    ../../../src/opengl/glContext.cpp \
    ../../../src/opengl/glFont.cpp \
    ../../../src/opengl/glImage.cpp \
    ../../../src/opengl/glPaint.cpp \
    ../../../src/opengl/glPath.cpp \
    ../../../src/mkBaseObject.cpp \
    ../../../src/mkBatch.cpp \
    ../../../src/mkContext.cpp \
    ../../../src/mkFont.cpp \
    ../../../src/mkImage.cpp \
    ../../../src/mkMath.cpp \
    ../../../src/mkPaint.cpp \
    ../../../src/mkParameter.cpp \
    ../../../src/mkPath.cpp \
    ../../../src/mkVGU.cpp

LOCAL_LDLIBS    := -llog -lGLESv1_CM

LOCAL_CFLAGS += -I$(LOCAL_PATH)/boost/include/ 
LOCAL_LDLIBS += -L$(LOCAL_PATH)/boost/lib/ -lboost_system-gcc-mt

include $(BUILD_SHARED_LIBRARY)
