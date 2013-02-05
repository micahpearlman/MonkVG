//
//  OpenGLESPlatform.h
//  MonkVG-OSX
//
//  Created by Micah Pearlman on 2/3/13.
//
//

#ifndef MonkVG_OSX_OpenGLESPlatform_h
#define MonkVG_OSX_OpenGLESPlatform_h


#if defined(__APPLE__)

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #if INCLUDE_ES1_HEADERS
        #include <OpenGLES/ES1/gl.h>
        #include <OpenGLES/ES1/glext.h>
    #else
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #endif // INCLUDE_ES1_HEADERS
    #define PLATFORM_IS_GLES 1
#else   // OSX
    #include <OpenGL/glu.h>
    #define GL_GLEXT_PROTOTYPES 1
    #define PLATFORM_IS_GLES 0
    #include <OpenGL/glext.h>

#endif

#elif ANDROID
    #define PLATFORM_IS_GLES 1
    #define GL_GLEXT_PROTOTYPES 1
    #if INCLUDE_ES1_HEADERS
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #else
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #endif // #if INCLUDE_ES1_HEADERS
    #include "glu.h"

#elif WIN32
    #define PLATFORM_IS_GLES 0
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <stdint.h>
    #include <GL/glew.h>
    #pragma comment(lib,"glew32.lib")

#elif __linux__

#if defined(USE_OPENGL)
    #define PLATFORM_IS_GLES 0
    #define GL_GLEXT_PROTOTYPES 1
    #include <sys/types.h>
    #include <stdlib.h>
    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/glu.h>

#else
    #define PLATFORM_IS_GLES 1
    #define PLATFORM_IS_MESA 1
    #define GL_GLEXT_PROTOTYPES 1
    #if INCLUDE_ES1_HEADERS
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #else
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #endif // #if INCLUDE_ES1_HEADERS
#endif // #if defined(USE_OPENGL)

#endif // #if defined(__APPLE__)

#ifndef APIENTRY
    #define APIENTRY
#endif // APIENTRY



#endif
