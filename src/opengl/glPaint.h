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

namespace MonkVG {
	
	class OpenGLPaint : public IPaint {
	public:
		OpenGLPaint();
		bool isDirty() { return _isDirty; }
		void setIsDirty( bool b ) { _isDirty = b; }
		void setGLState();
		
	private:
		bool	_isDirty;
		
	};
}
#endif // __qzPaint_h__