/*
 *  glContext.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glContext_h__
#define __glContext_h__

#include "mkContext.h"

namespace MonkVG {
	
	class OpenGLContext : public IContext {
	public:
	
		OpenGLContext();
		
		virtual bool Initialize();
		virtual bool Terminate();
		
		//// factories ////
		virtual IPath* createPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities );
		virtual void destroyPath( IPath* path );		
		virtual IPaint* createPaint();

		//// platform specific execution of stroke and fill ////
		virtual void stroke();
		virtual void fill();
		
		//// platform specific execution of Masking and Clearing ////
		virtual void clear(VGint x, VGint y, VGint width, VGint height);
		
		//// platform specific implementation of transform ////
		virtual void setIdentity();
		virtual void transform();
		virtual void scale( VGfloat sx, VGfloat sy );
		virtual void translate( VGfloat x, VGfloat y );
		virtual void rotate( VGfloat angle );
		
		
	private:
	
	};
}

#endif // __qlContext_h__