# name it as libOpenVG.so

# For Debug build.
#LTCFLAGS=-O0 -g3 -DOPENGLES_DEBUG
top = ${top_srcdir}/../..

lib_LTLIBRARIES = libOpenVG.la 

gles2bc_dir=${top}/thirdparty/gles2-bc/Sources/OpenGLES
libOpenVG_la_SOURCES=../../src/mkBaseObject.cpp  ../../src/mkContext.cpp  ../../src/mkImage.cpp  ../../src/mkPaint.cpp      ../../src/mkPath.cpp \
					  ../../src/mkBatch.cpp       ../../src/mkFont.cpp     ../../src/mkMath.cpp   ../../src/mkParameter.cpp

libOpenVG_la_SOURCES+=../../src/opengl/glBatch.cpp  ../../src/opengl/glContext.cpp  ../../src/opengl/glFont.cpp  ../../src/opengl/glImage.cpp  ../../src/opengl/glPaint.cpp  ../../src/opengl/glPath.cpp

libOpenVG_la_SOURCES+=../../glu/libtess/tess.c ../../glu/libtess/sweep.c ../../glu/libtess/normal.c ../../glu/libtess/geom.c ../../glu/libtess/dict.c ../../glu/libtess/mesh.c \
			../../glu/libtess/render.c ../../glu/libtess/priorityq.c ../../glu/libtess/memalloc.c ../../glu/libtess/tessmono.c ../../glu/libutil/project.c ../../glu/libutil/registry.c \
			../../glu/libutil/glue.c ../../glu/libutil/error.c

libOpenVG_la_SOURCES += ${gles2bc_dir}/OpenGLESConfig.cpp ${gles2bc_dir}/OpenGLESContext.cpp ${gles2bc_dir}/OpenGLESFile.cpp ${gles2bc_dir}/OpenGLESImplementation.cpp \
  ${gles2bc_dir}/OpenGLESString.cpp ${gles2bc_dir}/OpenGLESUtil.cpp ${gles2bc_dir}/OpenGLES11/OpenGLES11Context.cpp ${gles2bc_dir}/OpenGLES11/OpenGLES11Implementation.cpp \
  ${gles2bc_dir}/OpenGLES20/Attribute.cpp ${gles2bc_dir}/OpenGLES20/MatrixStack.cpp ${gles2bc_dir}/OpenGLES20/OpenGLES20Context.cpp \
  ${gles2bc_dir}/OpenGLES20/OpenGLES20Implementation.cpp ${gles2bc_dir}/OpenGLES20/OpenGLESState.cpp ${gles2bc_dir}/OpenGLES20/Shader.cpp ${gles2bc_dir}/OpenGLES20/ShaderFile.cpp \
  ${gles2bc_dir}/OpenGLES20/ShaderProgram.cpp  ${gles2bc_dir}/OpenGLES20/ShaderSource.cpp ${gles2bc_dir}/OpenGLES20/Uniform.cpp

libOpenVG_la_CPPFLAGS=-DUSE_SYSTEM_FMEMOPEN -I${top}/glu/include -I${top}/include -I${top}/src -I${top}/src/opengl -I${top}/thirdparty/gles2-bc/Sources/OpenGLES -I${top}/thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES11 -I${top}/thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES20

# For GLES linking, uncomment this...
libOpenVG_la_LIBADD=-lGLESv2 -lGLESv1_CM
# ...or for desktop GL linking, uncomment this...
#libOpenVG_la_LIBADD=-lGL -lGLU
#libOpenVG_la_CPPFLAGS+=-DUSE_OPENGL

libOpenVG_la_LDFLAGS=-version-info 1:1

# name it as libOpenVGU.so
lib_LTLIBRARIES+=libOpenVGU.la

libOpenVGU_la_SOURCES=../../src/mkVGU.cpp


libOpenVGU_la_CPPFLAGS=${libOpenVG_la_CPPFLAGS}


libOpenVGU_la_LIBADD=${libOpenVG_la_LIBADD}

libOpenVGU_la_LDFLAGS=-version-info 1:1


# install headers to /usr/include
library_includedir=$(includedir)
library_include_HEADERS=../../include/MonkVG/openvg.h  ../../include/MonkVG/vgext.h  ../../include/MonkVG/vgplatform.h  ../../include/MonkVG/vgu.h

