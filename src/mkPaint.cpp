/*
 *  mkPaint.cpp
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/3/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "mkPaint.h"
#include "mkContext.h"

namespace MonkVG {	// Internal Implementation
	VGint IPaint::getParameteri( const VGint p ) const {
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	VGfloat IPaint::getParameterf( const VGint p ) const {
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	void IPaint::getParameterfv( const VGint p, VGfloat *fv ) const {
		IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
	}

	void IPaint::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IPaint::setParameter( const VGint p, const VGfloat v ) 
	{
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IPaint::setParameter( const VGint p, const VGfloat* fv ) {
		switch (p) {
			case VG_PAINT_COLOR:
				for( int i = 0; i < 4; i++ )
					_paintColor[i] = fv[i];
				break;

			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	

	
}


///// OpenVG API Implementation /////

using namespace MonkVG;

VG_API_CALL VGPaint vgCreatePaint(void)
{
	return (VGPaint)IContext::instance().createPaint();
}

VG_API_CALL void vgDestroyPaint(VGPaint paint)
{
	// todo
}

VG_API_CALL void vgSetPaint(VGPaint paint, VGbitfield paintModes)
{
	// Set stroke / fill 
	if (paintModes & VG_STROKE_PATH)
		IContext::instance().setStrokePaint( (IPaint*)paint );
	if (paintModes & VG_FILL_PATH)
		IContext::instance().setFillPaint( (IPaint*)paint );
}

VG_API_CALL void vgPaintPattern(VGPaint paint, VGImage pattern)
{
// todo	((SHPaint*)paint)->pattern = pattern;
}
