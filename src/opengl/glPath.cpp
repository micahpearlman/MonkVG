/*
 *  glPath.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */


#include "glPath.h"
#include "glContext.h"

namespace MonkVG {
	
	bool OpenGLPath::draw( VGbitfield paintModes ) {
	
		// get the native OpenGL context
		OpenGLContext &OpenGLContext = (MonkVG::OpenGLContext&)IContext::instance();


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
				} break;
				case (VG_MOVE_TO >> 1):
				{	
					x = *coordsIter; coordsIter++;
					y = *coordsIter; coordsIter++;
				} break;
				case (VG_LINE_TO >> 1):
				{
					x = *coordsIter; coordsIter++;
					y = *coordsIter; coordsIter++;
				} break;
				case (VG_HLINE_TO >> 1):
				{
					x = *coordsIter; coordsIter++;
				} break;
				case (VG_VLINE_TO >> 1):
				{
					y = *coordsIter; coordsIter++;
				} break;
				case (VG_CUBIC_TO >> 1):
				{
					VGfloat cp1x = *coordsIter; coordsIter++;
					VGfloat cp1y = *coordsIter; coordsIter++;
					VGfloat cp2x = *coordsIter; coordsIter++;
					VGfloat cp2y = *coordsIter; coordsIter++;
					x = *coordsIter; coordsIter++;
					y = *coordsIter; coordsIter++;
				}
					break;	
				default:
					break;
			}
		}	// foreach segment
		
		
		Matrix33 active = *IContext::instance().getActiveMatrix();
		// a	b	0
		// c	d	0
		// tx	ty	1
		active.transpose();		// NOTE:  have to transpose.  Maybe should set up the matrices already as transposed?
		
		return true;
	}
}