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
LOCAL_PATH := $(call my-dir)
#cflags := -DOPENGLES_DEBUG
top_srcdir := $(call my-dir)/../../..
c_includes := \
		$(top_srcdir)/glu/include $(top_srcdir)/include \
		$(top_srcdir)/src $(top_srcdir)/src/opengl \
		$(top_srcdir)/thirdparty/gles2-bc/Sources/OpenGLES \
		$(top_srcdir)/thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES11 \
		$(top_srcdir)/thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES20 \
		$(top_srcdir)/thirdparty/fmemopen $(LOCAL_PATH)/boost/include
gles2bc_dir := ../../../thirdparty/gles2-bc/Sources/OpenGLES
glu_files := \
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
    ../../../glu/libutil/registry.c
src_files := \
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
    ../../../src/mkPath.cpp
thirdparty_files := \
	$(gles2bc_dir)/OpenGLESConfig.cpp \
	$(gles2bc_dir)/OpenGLESContext.cpp \
	$(gles2bc_dir)/OpenGLESFile.cpp \
	$(gles2bc_dir)/OpenGLESImplementation.cpp \
    $(gles2bc_dir)/OpenGLESString.cpp \
	$(gles2bc_dir)/OpenGLESUtil.cpp \
	$(gles2bc_dir)/OpenGLES11/OpenGLES11Context.cpp \
	$(gles2bc_dir)/OpenGLES11/OpenGLES11Implementation.cpp \
    $(gles2bc_dir)/OpenGLES20/Attribute.cpp \
	$(gles2bc_dir)/OpenGLES20/MatrixStack.cpp \
	$(gles2bc_dir)/OpenGLES20/OpenGLES20Context.cpp \
    $(gles2bc_dir)/OpenGLES20/OpenGLES20Implementation.cpp \
	$(gles2bc_dir)/OpenGLES20/OpenGLESState.cpp \
	$(gles2bc_dir)/OpenGLES20/Shader.cpp \
	$(gles2bc_dir)/OpenGLES20/ShaderFile.cpp \
    $(gles2bc_dir)/OpenGLES20/ShaderProgram.cpp \
	$(gles2bc_dir)/OpenGLES20/ShaderSource.cpp \
	$(gles2bc_dir)/OpenGLES20/Uniform.cpp

include $(CLEAR_VARS)
LOCAL_MODULE     := libOpenVG
LOCAL_CFLAGS     := $(cflags)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_LDLIBS     := -llog -lGLESv1_CM -lGLESv2
LOCAL_SRC_FILES  := $(glu_files) $(src_files) $(thirdparty_files)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE     := libOpenVGU
LOCAL_CFLAGS     := $(cflags)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_LDLIBS     := -llog -lGLESv1_CM -lGLESv2
LOCAL_SRC_FILES  := ../../../src/mkVGU.cpp
LOCAL_STATIC_LIBRARIES := libOpenVG
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE     := libOpenVGU-shared
LOCAL_CFLAGS     := $(cflags)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_LDLIBS     := -llog -lGLESv1_CM -lGLESv2
LOCAL_SRC_FILES  := 
LOCAL_STATIC_LIBRARIES := libOpenVGU libOpenVG 
include $(BUILD_SHARED_LIBRARY)


