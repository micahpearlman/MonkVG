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
#else // #if defined(__APPLE__)
#if ANDROID
#include "glu.h"
#endif
#endif // #if defined(__APPLE__)



#endif
