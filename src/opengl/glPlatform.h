//
//  glPlatform.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 8/6/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#ifndef MonkVG_iOS_glPlatform_h
#define MonkVG_iOS_glPlatform_h

#if defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#include "glu.h"
#else
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
//#include <GL/freeglut.h>
#pragma comment(lib,"glew32.lib")
//#include <gl\gl.h>		// Header File For The OpenGL32 Library
//#include <gl\glu.h>		// Header File For The GLu32 Library	
#endif // #if defined(__APPLE__)

#ifndef APIENTRY
#define APIENTRY
#endif // APIENTRY

#endif
