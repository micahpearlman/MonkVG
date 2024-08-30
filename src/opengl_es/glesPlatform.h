//
//  glPlatform.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 8/6/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#ifndef __glesPlatform_h__
#define __glesPlatform_h__

#if defined(__APPLE__)
    // turn off opengl deprecated warnings
    #define GL_SILENCE_DEPRECATION

    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #include "glu.h"
    #else   // OSX
        #include <OpenGL/glu.h>
        #define glOrthof glOrtho
    #endif

#elif ANDROID

    #include "glu.h"

#elif WIN32

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
//  #include <GL/glew.h>
    #define glOrthof glOrthofOES
    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/glu.h>

#else
    #define PLATFORM_IS_GLES 1
    #define GL_GLEXT_PROTOTYPES 1
    #if INCLUDE_ES1_HEADERS
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #else
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #endif // #if INCLUDE_ES1_HEADERS

    #include <GL/glu.h>
#endif // #if defined(USE_OPENGL)


#endif // #if defined(__APPLE__)

#ifndef APIENTRY
    #define APIENTRY
#endif // APIENTRY

#endif // __glesPlatform_h__
