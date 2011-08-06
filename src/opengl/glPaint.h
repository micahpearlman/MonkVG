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

namespace MonkVG {
	
	class OpenGLPaint : public IPaint {
	public:
		OpenGLPaint();
		void setGLState();
		void buildLinearGradientImage();
		virtual bool isDirty() { return _isDirty; }
		virtual void setIsDirty( bool b ) { _isDirty = b; }
		
	private:
		bool		_isDirty;
		VGImage		_gradientImage;
		
		
	};
}
#endif // __qzPaint_h__