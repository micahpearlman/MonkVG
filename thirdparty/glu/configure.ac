# Copyright © 2011 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

AC_PREREQ([2.60])
AC_INIT([glu],
        [9.0.2],
        [https://gitlab.freedesktop.org/mesa/glu/issues])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([dist-xz foreign subdir-objects])

LT_PREREQ([2.2])
LT_INIT

dnl Check for progs
AC_PROG_CPP
AC_PROG_CC
AC_PROG_CXX

dnl Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],
                [Enable debugging information]),
              [if test x$enableval = xyes; then
               CFLAGS="$CFLAGS -g -O0 -DDEBUG"
               CXXFLAGS="$CXXFLAGS -g -O0 -DDEBUG"; else
               CFLAGS="$CFLAGS -DNDEBUG"
               CXXFLAGS="$CXXFLAGS -DNDEBUG"; fi],
              [CFLAGS="$CFLAGS -DNDEBUG"
               CXXFLAGS="$CXXFLAGS -DNDEBUG"])

dnl Make sure the pkg-config macros are defined
m4_ifndef([PKG_PROG_PKG_CONFIG],
    [m4_fatal([Could not locate the pkg-config autoconf macros.
  These are usually located in /usr/share/aclocal/pkg.m4. If your macros
  are in a different location, try setting the environment variable
  ACLOCAL="aclocal -I/other/macro/dir" before running autoreconf.])])
PKG_PROG_PKG_CONFIG()

AC_ARG_ENABLE(libglvnd,
              AS_HELP_STRING([--enable-libglvnd],
                [Enable use of libglvnd]),
              [LIBGLVND="$enableval"],
              [LIBGLVND=no])

AC_ARG_ENABLE(osmesa,
              AS_HELP_STRING([--enable-osmesa],
                [Enable use of OSMesa instead of libGL]),
              [OSMESA="$enableval"],
              [OSMESA=no])

dnl Get the pkg-config definitions for libglvnd/OSMesa/libGL.  We include a
dnl fallback path for implementations that don't provide a .pc file
AS_IF([test "x$LIBGLVND" = "xyes"], [
    GLU_REQUIRES="opengl"
    PKG_CHECK_MODULES(LIBGLVND, [opengl], [], [
        AC_CHECK_HEADER([GL/gl.h],
                        [],
                        AC_MSG_ERROR([libglvnd GL not found]))
        AC_CHECK_LIB([OpenGL],
                     [glBegin],
                     [GL_LIBS=-lOpenGL],
                     AC_MSG_ERROR([libglvnd libOpenGL required]))
    ])
], [test "x$OSMESA" = "xyes"], [
    GLU_REQUIRES="osmesa"
    PKG_CHECK_MODULES(OSMESA, [osmesa], [], [
        AC_CHECK_LIB([OSMesa],
                     [glBegin],
                     [OSMESA_LIBS=-lOSMesa],
                     AC_MSG_ERROR([OSMesa required]))
    ])
], [
    GLU_REQUIRES="gl"
    PKG_CHECK_MODULES(GL, [gl], [], [
        AC_CHECK_HEADER([GL/gl.h],
                        [],
                        AC_MSG_ERROR([GL not found]))
        AC_CHECK_LIB([GL],
                     [glBegin],
                     [GL_LIBS=-lGL],
                     AC_MSG_ERROR([GL required]))
    ])
])

AC_SUBST([GLU_REQUIRES])

dnl Set up C warning and visibility flags.
if test "x$GCC" = xyes; then
    WARNCFLAGS="-Wall"

    # Enable -fvisibility=hidden if using a gcc that supports it
    save_CFLAGS="$CFLAGS"
    AC_MSG_CHECKING([whether $CC supports -fvisibility=hidden])
    VISIBILITY_CFLAGS="-fvisibility=hidden"
    CFLAGS="$CFLAGS $VISIBILITY_CFLAGS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM()], AC_MSG_RESULT([yes]),
                   [VISIBILITY_CFLAGS=""; AC_MSG_RESULT([no])]);

    # Restore CFLAGS; VISIBILITY_CFLAGS are added to it where needed.
    CFLAGS=$save_CFLAGS

    if test "x$GXX" = xyes; then
        WARNCXXFLAGS="-Wall"

        # Enable -fvisibility=hidden if using a gcc that supports it
        save_CXXFLAGS="$CXXFLAGS"
        AC_MSG_CHECKING([whether $CXX supports -fvisibility=hidden])
        VISIBILITY_CXXFLAGS="-fvisibility=hidden"
        CXXFLAGS="$CXXFLAGS $VISIBILITY_CXXFLAGS"
        AC_LANG_PUSH([C++])
        AC_LINK_IFELSE([AC_LANG_PROGRAM()], AC_MSG_RESULT([yes]),
                       [VISIBILITY_CXXFLAGS=""; AC_MSG_RESULT([no])]);
        AC_LANG_POP([C++])

        # Restore CXXFLAGS; VISIBILITY_CXXFLAGS are added to it where needed.
        CXXFLAGS=$save_CXXFLAGS
    fi
else
    AC_CHECK_DECL([__SUNPRO_C], [SUNCC="yes"], [SUNCC="no"])
    if test "x$SUNCC" = "xyes"; then
        WARNCFLAGS="-v"
        WARNCXXFLAGS="-v"
    fi
fi
AC_SUBST([WARNCFLAGS])
AC_SUBST([WARNCXXFLAGS])
AC_SUBST([VISIBILITY_CFLAGS])
AC_SUBST([VISIBILITY_CXXFLAGS])

AC_OUTPUT([
    Makefile
    glu.pc
])
