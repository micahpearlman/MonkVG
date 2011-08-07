/*
 *  glPaint.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glPaint.h"
#include <cmath>
#include <assert.h>

using namespace std;

namespace MonkVG {
	OpenGLPaint::OpenGLPaint()
	:	IPaint()
	,	_isDirty( true )
	,	_gradientImage( VG_INVALID_HANDLE )
	{
		
	}
	
	OpenGLPaint::~OpenGLPaint() {
		if ( _gradientImage != VG_INVALID_HANDLE ) {
			vgDestroyImage( _gradientImage );
		}
	}
	
	void OpenGLPaint::setGLState() {
		if ( isDirty() ) {
			const VGfloat* c = getPaintColor();
			glColor4f(c[0], c[1], c[2], c[3] );
		}
	}
	
	void OpenGLPaint::buildLinearGradientImage( VGfloat pathWidth, VGfloat pathHeight ) {
		// generated image sizes
		const int width = 64, height = 64;
		unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
		//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
		//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
		//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,
		
		VGColorRampSpreadMode spreadMode = _colorRampSpreadMode;
		
		const int stopCnt = _colorRampStops.size();
		
		//	from OpenVG specification PDF
		//			dx(x - x0) + dy((y - y0)
		// g(x,y) = ------------------------
		//				dx^2 + dy^2
		// where dx = x1 - x0, dy = y1 - y0
		//
		VGfloat p0[2] = { (_paintLinearGradient[0]/pathWidth) * width, (_paintLinearGradient[1]/pathHeight) * height};
		VGfloat p1[2] = { (_paintLinearGradient[2]/pathWidth) * width, (_paintLinearGradient[3]/pathHeight) * height};
		
		
		float dx = p1[0] - p0[0];
		float dy = p1[1] - p0[1];
		float denominator = (dx * dx) + (dy * dy);
		// todo: assert denominator != 0
		
		for ( int x = 0; x < width; x++ ) {
			for ( int y = 0; y < height; y++ ) {
				float numerator = dx * (x - p0[0]) + dy * (y - p0[1]);
				float g = numerator / denominator;
				
				
				
				// color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
				// where c0 = stop color 0, c1 = stop color 1
				// where x0 = stop offset 0, x1 = stop offset 1
				float finalcolor[4];
				float* stop0 = 0;
				float* stop1 = 0;
				
				
				if ( spreadMode == VG_COLOR_RAMP_SPREAD_PAD ) {
					if ( g < 0 ) {
						stop0 = _colorRampStops[0].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
						
					} else if( g > 1 ) {
						stop0 = _colorRampStops[stopCnt -1].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
						
					} else {
						// determine which stops
						for ( int i = 0; i < stopCnt; i++ ) {
							if ( g >= _colorRampStops[i].a[0] && g < _colorRampStops[i+1].a[0] ) {
								stop0 = _colorRampStops[i].a;
								stop1 = _colorRampStops[i+1].a;
								//printf( "stopds: %d --> %d\n", i, i+1);
								break;
							}
						}
						
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1] + (stop1[i+1] - stop0[i+1])*(g - stop0[0])/(stop1[0] - stop0[0]);
						}
						
					}
				} else {
					//g = fabsf(g);
					int w = int(fabsf(g));
					
					if ( spreadMode == VG_COLOR_RAMP_SPREAD_REPEAT ) {
						if ( g < 0 ) {
							g = 1 - (fabs(g) - w);
						} else {
							g = g - w;
						}
					} else if( spreadMode == VG_COLOR_RAMP_SPREAD_REFLECT ) {
						if ( g < 0 ) {
							if ( w % 2 == 0 ) { // even
								g = (fabsf(g) - w);
							} else {	// odd
								g = (1 - (fabsf(g) - w));
							}
							
						} else {
							if ( w % 2 == 0 ) { // even
								g = g - w;
							} else {	// odd
								g = 1 - (g - w);
							}
						}
						
					}
					
					// clamp
					if ( g > 1 ) {
						g = 1;
					}
					if ( g < 0 ) {
						g = 0;
					}
					
					// determine which stops
					for ( int i = 0; i < stopCnt; i++ ) {
						if ( g >= _colorRampStops[i].a[0] && g <= _colorRampStops[i+1].a[0] ) {
							stop0 = _colorRampStops[i].a;
							stop1 = _colorRampStops[i+1].a;
							//printf( "stopds: %d --> %d\n", i, i+1);
							break;
						}
					}
					
					assert( stop0 && stop1 );
					for ( int i = 0; i < 4; i++ ) {
						finalcolor[i] = stop0[i+1] + (stop1[i+1] - stop0[i+1])*(g - stop0[0])/(stop1[0] - stop0[0]);
					}
				}
				
				unsigned int color 
				= (uint32_t(finalcolor[3] * 255) << 24) 
				| (uint32_t(finalcolor[2] * 255) << 16)
				| (uint32_t(finalcolor[1] * 255) << 8)
				| (uint32_t(finalcolor[0] * 255) << 0);
				
				image[(y*width) + x] = color;
			}
		}
		
		// create openvg image
		_gradientImage = vgCreateImage(VG_sRGBA_8888, width, height, 0 );
		
		vgImageSubData( _gradientImage, image, -1, VG_sRGBA_8888, 0, 0, width, height );
		
		free(image);

	}
}