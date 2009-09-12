/*
 *  qzPath.cpp
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/3/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "qzPath.h"
#include "qzContext.h"

namespace MonkVG {
	
	bool QuartzPath::draw( VGbitfield paintModes ) {
	
		// get the native quartz context
		QuartzContext &quartzContext = (QuartzContext&)IContext::instance();
		CGContextRef context = quartzContext.getNativeContext();

//		quartzContext.transform();
		
		// start up a path in the native context
		CGContextBeginPath( context );
		
		vector< VGfloat >::iterator coordsIter = _fcoords->begin();
		int numCoords = 0;
		VGbyte segment = VG_CLOSE_PATH;
		VGfloat x = 0.0f, y = 0.0f;
		
		// setup stroking
		if( paintModes & VG_STROKE_PATH )
		{
			IContext::instance().stroke();
		}
		// setup fill
		if( paintModes & VG_FILL_PATH ) 
		{
			IContext::instance().fill();
		}
		
		
		for ( vector< VGubyte >::iterator segmentIter = _segments.begin(); segmentIter != _segments.end(); segmentIter++ ) {
			segment = (*segmentIter);
			numCoords = segmentToNumCoordinates( static_cast<VGPathSegment>( segment ) );
			//segment = segment >> 1;

			
//			VG_CLOSE_PATH                               = ( 0 << 1),
//			VG_MOVE_TO                                  = ( 1 << 1),
//			VG_LINE_TO                                  = ( 2 << 1),
//			VG_HLINE_TO                                 = ( 3 << 1),
//			VG_VLINE_TO                                 = ( 4 << 1),
//			VG_QUAD_TO                                  = ( 5 << 1),
//			VG_CUBIC_TO                                 = ( 6 << 1),
//			VG_SQUAD_TO                                 = ( 7 << 1),
//			VG_SCUBIC_TO                                = ( 8 << 1),
//			VG_SCCWARC_TO                               = ( 9 << 1),
//			VG_SCWARC_TO                                = (10 << 1),
//			VG_LCCWARC_TO                               = (11 << 1),
//			VG_LCWARC_TO                                = (12 << 1),
			
			// todo: deal with relative move
			switch (segment >> 1) {
				case (VG_CLOSE_PATH > 1):
				{
					CGContextClosePath( context );
				} break;
				case (VG_MOVE_TO >> 1):
				{	
					x = *coordsIter; coordsIter++;
					y = *coordsIter; coordsIter++;
					CGContextMoveToPoint ( context, x, y );
				} break;
				case (VG_LINE_TO >> 1):
				{
					x = *coordsIter; coordsIter++;
					y = *coordsIter; coordsIter++;
					CGContextAddLineToPoint( context, x, y );
				} break;
				case (VG_HLINE_TO >> 1):
				{
					x = *coordsIter; coordsIter++;
					CGContextAddLineToPoint( context, x, y );
					
				} break;
				case (VG_VLINE_TO >> 1):
				{
					y = *coordsIter; coordsIter++;
					CGContextAddLineToPoint( context, x, y );
					
				} break;
				case (VG_CUBIC_TO >> 1):
				{
					VGfloat cp1x = *coordsIter; coordsIter++;
					VGfloat cp1y = *coordsIter; coordsIter++;
					VGfloat cp2x = *coordsIter; coordsIter++;
					VGfloat cp2y = *coordsIter; coordsIter++;
					x = *coordsIter; coordsIter++;
					y = *coordsIter; coordsIter++;
					CGContextAddCurveToPoint ( context, cp1x, cp1y, cp2x, cp2y, x, y );
				}
				break;	
				default:
					break;
			}
		}	// foreach segment
		
		
		
		CGPathDrawingMode mode;
		if( (paintModes & VG_STROKE_PATH) && (paintModes & VG_FILL_PATH) ) {
			if( quartzContext.getFillRule() == VG_NON_ZERO )
				mode = kCGPathFillStroke;
			else
				mode = kCGPathEOFillStroke;
		}
		else if( paintModes & VG_STROKE_PATH ) {
			mode = kCGPathStroke;
		}
		else {
			if( quartzContext.getFillRule() == VG_NON_ZERO )		
				mode = kCGPathFill;
			else
				mode = kCGPathEOFill;
		}

		
		// draw the current path in the context.  remember that there can only ever be one path in a given context
		CGContextDrawPath ( context, mode );
		
		return true;
	}
}