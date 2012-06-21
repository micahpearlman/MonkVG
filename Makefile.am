# name it as libOpenVG.so

lib_LTLIBRARIES = libOpenVG.la 

libOpenVG_la_SOURCES=src/mkBaseObject.cpp  src/mkContext.cpp  src/mkImage.cpp  src/mkPaint.cpp      src/mkPath.cpp \
					  src/mkBatch.cpp       src/mkFont.cpp     src/mkMath.cpp   src/mkParameter.cpp

libOpenVG_la_SOURCES+=src/opengl/glBatch.cpp  src/opengl/glContext.cpp  src/opengl/glFont.cpp  src/opengl/glImage.cpp  src/opengl/glPaint.cpp  src/opengl/glPath.cpp


libOpenVG_la_CPPFLAGS=-I${top_srcdir}/include -I${top_srcdir}/src -I${top_srcdir}/src/opengl


libOpenVG_la_LIBADD=-lGL -lGLU -lGLEW

libOpenVG_la_LDFLAGS=-version-info 1:1

# name it as libOpenVGU.so
lib_LTLIBRARIES+=libOpenVGU.la

libOpenVGU_la_SOURCES=src/mkVGU.cpp


libOpenVGU_la_CPPFLAGS=-I${top_srcdir}/include -I${top_srcdir}/src -I${top_srcdir}/src/opengl


libOpenVGU_la_LIBADD=libOpenVG.la -lGL -lGLU -lGLEW

libOpenVGU_la_LDFLAGS=-version-info 1:1


# install headers to /usr/include
library_includedir=$(includedir)
library_include_HEADERS=include/MonkVG/openvg.h  include/MonkVG/vgext.h  include/MonkVG/vgplatform.h  include/MonkVG/vgu.h

