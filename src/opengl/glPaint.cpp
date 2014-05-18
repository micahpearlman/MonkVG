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
#include "glContext.h"

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
            GL->glColor4f( c[0], c[1], c[2], c[3] );
		}
	}

	void OpenGLPaint::lerpColor(float * dst, float * stop0, float * stop1, float g) {
		float den = std::max(0.00001f, stop1 != stop0? stop1[0] - stop0[0] : 1 - stop0[0]);
		for ( int i = 0; i < 4; i++ )
			dst[i] = stop0[i+1] + (stop1[i+1] - stop0[i+1])*(g - stop0[0])/den;
	}

	void OpenGLPaint::calcStops(float ** stop0, float ** stop1, float g) {
		assert(g >= 0);
		assert(g <= 1);
		size_t stopCnt = _colorRampStops.size();
		float * s0 = 0;
		float * s1 = 0;
		for ( size_t i = 0; i < stopCnt && !s1; i++ ) {
			float * curr = _colorRampStops[i].a;
			if ( g >= curr[0] )
				s0 = curr;
			else if ( s0 && g <= curr[0] )
				s1 = curr;
		}
		if ( stopCnt == 0 ) {
			static float implicit0[] = {0,0,0,1};
			static float implicit1[] = {1,1,1,1};
			s0 = implicit0;
			s1 = implicit1;
		} else {
			if ( !s0 )
				s0 = _colorRampStops[0].a;
			if ( !s1 )
				s1 = _colorRampStops[stopCnt - 1].a;
		}
		assert(s0[0] <= g && (g <= s1[0] || s1 == _colorRampStops[stopCnt-1].a));
		*stop0 = s0;
		*stop1 = s1;
	}
	
	void OpenGLPaint::buildLinearGradientImage( VGfloat pathWidth, VGfloat pathHeight ) {
		// generated image sizes
		const int width = 64, height = 64;
		unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
		//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
		//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
		//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,
		
		VGColorRampSpreadMode spreadMode = _colorRampSpreadMode;
		
		const int stopCnt = (int)_colorRampStops.size();
		
		//	from OpenVG specification PDF
		//
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
				float* stop0;
				float* stop1;
				
				
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
						calcStops(&stop0, &stop1, g);
						lerpColor(finalcolor, stop0, stop1, g);
					}
				} else {
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
					calcStops(&stop0, &stop1, g);
					lerpColor(finalcolor, stop0, stop1, g);
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
	
	void OpenGLPaint::buildRadialGradientImage( VGfloat pathWidth, VGfloat pathHeight ) {
		// generated image sizes
		const int width = 64, height = 64;
		unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
		//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
		//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
		//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,
		
		VGColorRampSpreadMode spreadMode = _colorRampSpreadMode;
		
		const int stopCnt = (int)_colorRampStops.size();
		
		//	from OpenVG specification PDF
		//
		// VG_PAINT_RADIAL_GRADIENT. { cx, cy, fx, fy, r }.
		//
		//					(dx * fx' + dy * fy') + sqrt( r^2 * (dx^2 + dy^2) - (dx * fy' - dy fx') ^ 2 )
		//		g(x,y)	=	-----------------------------------------------------------------------------
		//												r^2 - (fx'^2 + fy'^2)
		// where:
		//		fx' = fx - cx, fy' = fy - cy
		//		dx = x - fx, dy = y - fy
		//		
		
		
		// normalize the focal point
		float fxn = (( _paintRadialGradient[2])/pathWidth) * width;
		float fyn = ((_paintRadialGradient[3])/pathHeight) * height;
		float fxp = fxn - (((_paintRadialGradient[0])/pathWidth) * width);
		float fyp = fyn - (((_paintRadialGradient[1])/pathHeight) * height);
		
		// ??? normalizing radius on the path width but it could be either or???
		float rn = (_paintRadialGradient[4]/pathWidth) * width;
		
		float denominator = (rn*rn) - (fxp*fxp + fyp*fyp);

		
		for ( int x = 0; x < width; x++ ) {
			float dx = x - fxn;
			for ( int y = 0; y < height; y++ ) {
				float dy = y - fyn;
				
				float numerator = (dx * fxp + dy * fyp);
				float df = dx * fyp - dy * fxp;
				numerator += sqrtf( (rn*rn) * (dx*dx + dy*dy) - (df*df)  );
				float g = numerator / denominator;
				
				
				
				// color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
				// where c0 = stop color 0, c1 = stop color 1
				// where x0 = stop offset 0, x1 = stop offset 1
				float finalcolor[4];
				float* stop0;
				float* stop1;
				
				
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
						calcStops(&stop0, &stop1, g);
						lerpColor(finalcolor, stop0, stop1, g);
					}
				} else {
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
					
					calcStops(&stop0, &stop1, g);
					lerpColor(finalcolor, stop0, stop1, g);
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
    
    void OpenGLPaint::buildLinear2x3GradientImage( VGfloat pathWidth, VGfloat pathHeight ) {
		// generated image sizes
		const int width = 64, height = 64;
		unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
		//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
		//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
		//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,
		
		VGColorRampSpreadMode spreadMode = _colorRampSpreadMode;
		
		const int stopCnt = (int)_colorRampStops.size();
		
		//	from OpenVG specification PDF
		//
		//			dx(x - x0) + dy((y - y0)
		// g(x,y) = ------------------------
		//				dx^2 + dy^2
		// where dx = x1 - x0, dy = y1 - y0
		//
        
        /*
         Setup x & y values baed on gradient size:
            x|y = (scale * 32768 * 0.05)
         Two transformations:
            1. Flash transform, based on matrix:
                x' = x + y * r1 + tx
                y' = y + x * r0 + ty
            2. Conversion to OpenVG space:
                x'' = (x' / pathWidth) * width
                y'' = (y' / pathHeight) * height
         */
        
        // initialize gradient space
        float gradientSize = 32768 * 0.05;
		VGfloat p1[2] = { gradientSize/2, gradientSize/2};
		VGfloat p0[2] = { -p1[0], p1[1]};
        
        // flash trasformation based on matrix values
        VGfloat p0p[2] = {
            _paint2x3Gradient[0] * p0[0]  + p0[1] * _paint2x3Gradient[5],
            _paint2x3Gradient[1] * p0[1]  + p0[0] * _paint2x3Gradient[4]};
        VGfloat p1p[2] = {
            _paint2x3Gradient[0] * p1[0]  + p1[1] * _paint2x3Gradient[5],
            _paint2x3Gradient[1] * p1[1]  + p1[0] * _paint2x3Gradient[4]};
        
        // convert to the shapes space
        p0[0] = (p0p[0] + pathWidth/2) / pathWidth * width;
        p0[1] = (p0p[1] + pathHeight/2) / pathHeight * height;
        p1[0] = (p1p[0] + pathWidth/2) / pathWidth * width;
        p1[1] = (p1p[1] + pathHeight/2) / pathHeight * height;
        
        //float gradientWidth = _paint2x3Gradient[0] * 32768 * 0.05; // scale gradients
        //float delta = (pathWidth-gradientWidth) / pathWidth;
		//VGfloat p0[2] = { (delta/2) * width, 0};
		//VGfloat p1[2] = { p0[0] + ((gradientWidth / pathWidth) * width), 0};
		
		
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
				float* stop0;
				float* stop1;
				
				
				if ( spreadMode == VG_COLOR_RAMP_SPREAD_PAD ) {
					if ( g < 0 ) {
						stop0 = _colorRampStops[0].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
						
					} else if ( g < _colorRampStops[0].a[0]) { // if the first color is after 0 fill with the first color
                        stop0 = _colorRampStops[0].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
                    } else if( g > 1 ) {
						stop0 = _colorRampStops[stopCnt -1].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
						
					} else if ( g > _colorRampStops[stopCnt - 1].a[0]) { // if the last color is before 1.0 fill with the last color
                        stop0 = _colorRampStops[stopCnt -1].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
                    } else {
						calcStops(&stop0, &stop1, g);
						lerpColor(finalcolor, stop0, stop1, g);
					}
				} else {
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
					calcStops(&stop0, &stop1, g);
					lerpColor(finalcolor, stop0, stop1, g);
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
	
    void OpenGLPaint::buildRadial2x3GradientImage( VGfloat pathWidth, VGfloat pathHeight ) {
		// generated image sizes
		const int width = 64, height = 64;
		unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
		//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
		//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
		//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,
		
		VGColorRampSpreadMode spreadMode = _colorRampSpreadMode;
		
		const int stopCnt = (int)_colorRampStops.size();
		
		//	from OpenVG specification PDF
		//
		// VG_PAINT_RADIAL_GRADIENT. { cx, cy, fx, fy, r }.
		//
		//					(dx * fx' + dy * fy') + sqrt( r^2 * (dx^2 + dy^2) - (dx * fy' - dy fx') ^ 2 )
		//		g(x,y)	=	-----------------------------------------------------------------------------
		//												r^2 - (fx'^2 + fy'^2)
		// where:
		//		fx' = fx - cx, fy' = fy - cy
		//		dx = x - fx, dy = y - fy
		//		
		
		/*
         
         float fxn = (( _paintRadialGradient[2])/pathWidth) * width;
         float fyn = ((_paintRadialGradient[3])/pathHeight) * height;
         float fxp = fxn - (((_paintRadialGradient[0])/pathWidth) * width);
         float fyp = fyn - (((_paintRadialGradient[1])/pathHeight) * height);
         
         // ??? normalizing radius on the path width but it could be either or???
         float rn = (_paintRadialGradient[4]/pathWidth) * width;
         
         */
		// normalize the focal point
		float fxn = width/2;
		float fyn = height / 2;
		float fxp = fxn - (width/2);
		float fyp = fyn - (height/2);
		
		// ??? normalizing radius on the path width but it could be either or???
		float rn = width / 2;
		
		float denominator = (rn*rn) - (fxp*fxp + fyp*fyp);
        
		
		for ( int x = 0; x < width; x++ ) {
			float dx = x - fxn;
			for ( int y = 0; y < height; y++ ) {
				float dy = y - fyn;
				
				float numerator = (dx * fxp + dy * fyp);
				float df = dx * fyp - dy * fxp;
				numerator += sqrtf( (rn*rn) * (dx*dx + dy*dy) - (df*df)  );
				float g = numerator / denominator;
				
				
				
				// color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
				// where c0 = stop color 0, c1 = stop color 1
				// where x0 = stop offset 0, x1 = stop offset 1
				float finalcolor[4];
				float* stop0;
				float* stop1;
				
				
				if ( spreadMode == VG_COLOR_RAMP_SPREAD_PAD ) {
					if ( g < 0 ) {
						stop0 = _colorRampStops[0].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
						
					} else if ( g < _colorRampStops[0].a[0]) { // if the first color is after 0 fill with the first color
                        stop0 = _colorRampStops[0].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
                    } else if( g > 1 ) {
						stop0 = _colorRampStops[stopCnt -1].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
					} else if ( g > _colorRampStops[stopCnt - 1].a[0]) { // if the last color is before 1.0 fill with the last color
                        stop0 = _colorRampStops[stopCnt -1].a;
						for ( int i = 0; i < 4; i++ ) {
							finalcolor[i] = stop0[i+1];
						}
                    } else {
						calcStops(&stop0, &stop1, g);
						lerpColor(finalcolor, stop0, stop1, g);
						
					}
				} else {
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
					calcStops(&stop0, &stop1, g);
					lerpColor(finalcolor, stop0, stop1, g);
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
    
	void OpenGLPaint::buildGradientImage( VGfloat pathWidth, VGfloat pathHeight ) {
		if ( getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT ) {
			buildLinearGradientImage( pathWidth, pathHeight );
		} else if ( getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT ) {
			buildRadialGradientImage( pathWidth, pathHeight );
		} else if ( getPaintType() == VG_PAINT_TYPE_LINEAR_2x3_GRADIENT ) {
			buildLinear2x3GradientImage( pathWidth, pathHeight );
		}  else if ( getPaintType() == VG_PAINT_TYPE_RADIAL_2x3_GRADIENT ) {
			buildRadial2x3GradientImage( pathWidth, pathHeight );
		}
	}

}
