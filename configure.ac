#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MonkVG], [1.1],[] )
AC_CONFIG_SRCDIR([glu/libtess/dict.c])

# Checks for programs.
LT_INIT
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CXX
# Checks for libGL

AC_CHECK_HEADERS([ GL/gl.h GL/glu.h  GL/glew.h],[],[AC_MSG_FAILURE(need OpenGL)])

AC_CHECK_LIB(GL,glFlush, [], [AC_MSG_FAILURE(need OpenGL Library)])
AC_CHECK_LIB(GLU,gluTessBeginPolygon, [], [AC_MSG_FAILURE(need OpenGL Utility Library )])
AC_CHECK_LIB(GLEW,glOrtho, [], [AC_MSG_FAILURE(need OpenGL Extension Wrangler Library)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
