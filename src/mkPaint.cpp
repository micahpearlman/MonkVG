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
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	VGfloat IPaint::getParameterf( const VGint p ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	void IPaint::getParameterfv( const VGint p, VGfloat *fv ) const {
		switch (p) {
			case VG_PAINT_COLOR:
				for( int i = 0; i < 4; i++ )
					fv[i] = _paintColor[i];
				break;
	
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
		
	}

	void IPaint::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			case VG_PAINT_TYPE:
				setPaintType( (VGPaintType)v );
				break;
			case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
				_colorRampSpreadMode = (VGColorRampSpreadMode)v;
				break;
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IPaint::setParameter( const VGint p, const VGfloat v ) 
	{
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IPaint::setParameter( const VGint p, const VGfloat* fv, const VGint cnt ) {
		switch (p) {
			case VG_PAINT_COLOR:
				for( int i = 0; i < 4; i++ )
					_paintColor[i] = fv[i];
				break;

			case VG_PAINT_LINEAR_GRADIENT:
				for ( int i = 0; i < 4; i++ ) {
					_paintLinearGradient[i] = fv[i];
				}
				break;
			case VG_PAINT_RADIAL_GRADIENT:
				for ( int i = 0; i < 5; i++ ) {
					_paintRadialGradient[i] = fv[i];
				}
				break;
			case VG_PAINT_COLOR_RAMP_STOPS:
				for ( int j = 0; j < cnt/5; j++ ) {
					Stop_t stop;
					for ( int p = 0; p < 5; p++ ) {
						stop.a[p] = fv[(j * 5) + p];
					}
					_colorRampStops.push_back( stop );
				}
				break;
			case VG_PAINT_2x3_GRADIENT:
				for ( int i = 0; i < 6; i++ ) {
					_paint2x3Gradient[i] = fv[i];
				}
				break;
			
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	

	
}


///// OpenVG API Implementation /////

using namespace MonkVG;

VG_API_CALL VGPaint vgCreatePaint(void) {
	return (VGPaint)IContext::instance().createPaint();
}

VG_API_CALL void vgDestroyPaint(VGPaint paint) {
	IContext::instance().destroyPaint( (IPaint*)paint );
}

VG_API_CALL void vgSetPaint(VGPaint paint, VGbitfield paintModes) {
	if ( paint != VG_INVALID_HANDLE && ((IPaint*)paint)->getType() != BaseObject::kPaintType ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	if ( !paintModes || paintModes & ~(VG_FILL_PATH | VG_STROKE_PATH) ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	// Set stroke / fill 
	if (paintModes & VG_STROKE_PATH)
		IContext::instance().setStrokePaint( (IPaint*)paint );
	if (paintModes & VG_FILL_PATH)
		IContext::instance().setFillPaint( (IPaint*)paint );
}

VG_API_CALL VGPaint vgGetPaint(VGPaintMode paintModes) {
	if (paintModes & VG_STROKE_PATH)
		return (VGPaint)IContext::instance().getStrokePaint();
	if (paintModes & VG_FILL_PATH)
		return (VGPaint)IContext::instance().getFillPaint();
	SetError( VG_ILLEGAL_ARGUMENT_ERROR );
	return VG_INVALID_HANDLE;
}


VG_API_CALL void vgPaintPattern(VGPaint paint, VGImage pattern)
{
// todo	((SHPaint*)paint)->pattern = pattern;
}
