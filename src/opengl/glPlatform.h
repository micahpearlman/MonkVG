//
//  glPlatform.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 8/6/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#ifndef __GLPLATFORM_H__
#define __GLPLATFORM_H__

#ifndef GL_UNDEFINED
    #define GL_UNDEFINED 0
#endif // GL_UNDEFINED

#if defined(__APPLE__)
    // turn off opengl deprecated warnings
    #define GL_SILENCE_DEPRECATION

    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #error "iOS not supported. TODO: Add support for iOS"
        #include "glu.h"
    #else   // macos
        #include <OpenGL/gl3.h>
        #include <GL/glu.h>
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

#endif // __GLPLATFORM_H__
