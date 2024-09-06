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
	
	UIBezierPath* _uibezeirpath = nil;
	
	bool QuartzPath::draw( VGbitfield paintModes ) {
	
		// get the native quartz context
		QuartzContext &quartzContext = (QuartzContext&)IContext::instance();

		if ( _uibezeirpath == nil ) {
			_uibezeirpath = [UIBezierPath bezierPath];

			vector< VGfloat >::iterator coordsIter = _fcoords->begin();
			int numCoords = 0;
			VGbyte segment = VG_CLOSE_PATH;
			VGfloat x = 0.0f, y = 0.0f;
			
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
						[_uibezeirpath closePath];
					} break;
					case (VG_MOVE_TO >> 1):
					{	
						x = *coordsIter; coordsIter++;
						y = *coordsIter; coordsIter++;
						[_uibezeirpath moveToPoint:CGPointMake(x, y)];
					} break;
					case (VG_LINE_TO >> 1):
					{
						x = *coordsIter; coordsIter++;
						y = *coordsIter; coordsIter++;
						[_uibezeirpath addLineToPoint:CGPointMake(x, y)];
					} break;
					case (VG_HLINE_TO >> 1):
					{
						x = *coordsIter; coordsIter++;
						[_uibezeirpath addLineToPoint:CGPointMake(x, y)];
						
					} break;
					case (VG_VLINE_TO >> 1):
					{
						y = *coordsIter; coordsIter++;
						[_uibezeirpath addLineToPoint:CGPointMake(x, y)];
						
					} break;
					case (VG_CUBIC_TO >> 1):
					{
						VGfloat cp1x = *coordsIter; coordsIter++;
						VGfloat cp1y = *coordsIter; coordsIter++;
						VGfloat cp2x = *coordsIter; coordsIter++;
						VGfloat cp2y = *coordsIter; coordsIter++;
						x = *coordsIter; coordsIter++;
						y = *coordsIter; coordsIter++;
						[_uibezeirpath addCurveToPoint:CGPointMake(x, y) controlPoint1:CGPointMake(cp1x, cp1y) controlPoint2:CGPointMake(cp2x, cp2y)];
					}
						break;	
					default:
						break;
				}
			}	// foreach segment
			[_uibezeirpath closePath];
		}
		
		
		Matrix33 active = *IContext::instance().getActiveMatrix();
		// a	b	0
		// c	d	0
		// tx	ty	1
		active.transpose();		// NOTE:  have to transpose.  Maybe should set up the matrices already as transposed?
		
		CGAffineTransform transform = CGAffineTransformMake ( active.get( 0, 0 ), active.get( 0, 1 ),
												  active.get( 1, 0 ), active.get( 1, 1 ),
												  active.get( 2, 0 ), active.get( 2, 1 ) );
		[_uibezeirpath applyTransform:transform];
		
		if ( quartzContext.getFillRule() == VG_NON_ZERO ) {
			_uibezeirpath.usesEvenOddFillRule = NO;
		} else {
			_uibezeirpath.usesEvenOddFillRule = YES;
		}

		
		// setup fill
		if( paintModes & VG_FILL_PATH ) 
		{
			IContext::instance().fill();
			[_uibezeirpath fill];
		}
		if( paintModes & VG_STROKE_PATH )
		{
			IContext::instance().stroke();
			[_uibezeirpath stroke];
		}
		
		_uibezeirpath = nil;
		return true;
	}
}