/*
 *  glPath.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glPath_h__
#define __glPath_h__

#include "mkPath.h"
namespace MonkVG {
	
	class OpenGLPath : public IPath {
	public:
	
		OpenGLPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) 
			:	IPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities )
		{

		}
		virtual ~OpenGLPath() {
			
		}
		
		
		virtual bool draw( VGbitfield paintModes );
		
	private:
	};
}

#endif // __glPath_h__