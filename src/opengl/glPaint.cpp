/*
 *  glPaint.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glPaint.h"

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


namespace MonkVG {
	OpenGLPaint::OpenGLPaint()
	:	IPaint()
	,	_isDirty( true )
	{
		
	}
	
	void OpenGLPaint::setGLState() {
		if ( isDirty() ) {
			const VGfloat* c = getPaintColor();
			glColor4f(c[0], c[1], c[2], c[3] );
		}
	}
}