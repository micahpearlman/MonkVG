/*
 *  glPaint.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glPaint.h"


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
	
	void OpenGLPaint::buildLinearGradientImage() {
		
	}
}