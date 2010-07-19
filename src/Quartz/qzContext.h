/*
 *  qzContext.h
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/2/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __qzContext_h__
#define __qzContext_h__

#include "mkContext.h"
#include <QuartzCore/QuartzCore.h>
//#include <stack>

namespace MonkVG {
	
	class QuartzContext : public IContext {
	public:
	
		QuartzContext();
		
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
		
		
		CGContextRef getNativeContext();
		
	private:
	
		CGContextRef _context;		// the CoreGraphics context
									//		std::stack<bool>	_context_stack;
	
	};
}

#endif // __qzContext_h__