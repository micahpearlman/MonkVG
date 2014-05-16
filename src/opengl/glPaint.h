/*
 *  glPaint.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glPaint_h__
#define __glPaint_h__

#include "mkPaint.h"
#include "glPlatform.h"
#include "glImage.h"

namespace MonkVG {
	
	
	class OpenGLPaint : public IPaint {
	public:
		OpenGLPaint();
		virtual ~OpenGLPaint();
		void setGLState();
		void buildLinearGradientImage( VGfloat pathWidth, VGfloat pathHeight );
		void buildRadialGradientImage( VGfloat pathWidth, VGfloat pathHeight );
		void buildLinear2x3GradientImage( VGfloat pathWidth, VGfloat pathHeight );
		void buildRadial2x3GradientImage( VGfloat pathWidth, VGfloat pathHeight );
		void buildGradientImage( VGfloat pathWidth, VGfloat pathHeight );
		virtual bool isDirty() { return _isDirty; }
		virtual void setIsDirty( bool b ) { _isDirty = b; }
		
		OpenGLImage* getGradientImage() { return (OpenGLImage*)_gradientImage; }
		
	private:
		bool		_isDirty;
		VGImage		_gradientImage;
		
		void calcStops(float ** stop0, float ** stop1, float g);
		void lerpColor(float * dst, float * stop0, float * stop1, float g);
		
	};
}
#endif // __qzPaint_h__
