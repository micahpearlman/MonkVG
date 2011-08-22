//
//  ES1Renderer.m
//  MonkVG-Test-iOS-OpenGL
//
//  Created by Micah Pearlman on 8/11/10.
//  Copyright Zero Vision 2010. All rights reserved.
//

#import "ES1Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <map>
#include <vector>


extern "C" void loadTiger();
extern "C" void display(float dt);

using namespace std;

struct GlyphDescription {
	VGImage image;
	VGint ox, oy;	// offsets
	VGint height;
};

map<VGuint, GlyphDescription>	_glyphs;

VGImage buildRadialGradientImage() {
	// generated image sizes
	const int width = 64, height = 64;
	const float pathWidth = 64, pathHeight = 64;
	unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
	//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
	//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
	//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,
	
	VGColorRampSpreadMode spreadMode = VG_COLOR_RAMP_SPREAD_PAD;
	
	//const int stopCnt = _colorRampStops.size();
	const int stopCnt = 3;
	float _colorRampStops[stopCnt][5] = {
		{0.0f,	1.0f, 0.0f, 0.0f, 1.0f},
		{0.5f,	0.0f, 1.0f, 0.0f, 1.0f},
		{1.0f,	0.0f, 0.0f, 1.0f, 1.0f}
	};

	
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
	float _paintRadialGradient[5] = {32,32,16,16,32 };	// { cx, cy, fx, fy, r }.
	float fxn = (_paintRadialGradient[2]/pathWidth) * width;
	float fyn = (_paintRadialGradient[3]/pathHeight) * height;
	float fxp = fxn - ((_paintRadialGradient[0]/pathWidth) * width);
	float fyp = fyn - ((_paintRadialGradient[1]/pathHeight) * height);
	
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
			float* stop0 = 0;
			float* stop1 = 0;
			
			
			if ( spreadMode == VG_COLOR_RAMP_SPREAD_PAD ) {
				if ( g < 0 ) {
					stop0 = _colorRampStops[0];
					for ( int i = 0; i < 4; i++ ) {
						finalcolor[i] = stop0[i+1];
					}
					
				} else if( g > 1 ) {
					stop0 = _colorRampStops[stopCnt -1];
					for ( int i = 0; i < 4; i++ ) {
						finalcolor[i] = stop0[i+1];
					}
					
				} else {
					// determine which stops
					for ( int i = 0; i < stopCnt; i++ ) {
						if ( g >= _colorRampStops[i][0] && g <= _colorRampStops[i+1][0] ) {
							stop0 = _colorRampStops[i];
							stop1 = _colorRampStops[i+1];
							//printf( "stopds: %d --> %d\n", i, i+1);
							break;
						}
					}
					
					for ( int i = 0; i < 4; i++ ) {
						finalcolor[i] = stop0[i+1] + (stop1[i+1] - stop0[i+1])*(g - stop0[0])/(stop1[0] - stop0[0]);
					}
					
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
				
				// determine which stops
				for ( int i = 0; i < stopCnt; i++ ) {
					if ( g >= _colorRampStops[i][0] && g <= _colorRampStops[i+1][0] ) {
						stop0 = _colorRampStops[i];
						stop1 = _colorRampStops[i+1];
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
	VGImage _gradientImage = vgCreateImage(VG_sRGBA_8888, width, height, 0 );
	
	vgImageSubData( _gradientImage, image, -1, VG_sRGBA_8888, 0, 0, width, height );
	
	free(image);
	
	return _gradientImage;

}

VGImage buildLinearGradientImage() {

	// generated image sizes
	const int width = 64, height = 64;
	unsigned int* image = (unsigned int*)malloc( width * height * sizeof(unsigned int) );
	//	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
	//	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
	//	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,

	VGColorRampSpreadMode spreadMode = VG_COLOR_RAMP_SPREAD_REFLECT;
	
	const int stopCnt = 3;
	float stops[stopCnt][5] = {
		{0.0f,	1.0f, 0.0f, 0.0f, 1.0f},
		{0.5f,	0.0f, 1.0f, 0.0f, 1.0f},
		{1.0f,	0.0f, 0.0f, 1.0f, 1.0f}
	};
	

	//	from OpenVG specification PDF
	//			dx(x - x0) + dy((y - y0)
	// g(x,y) = ------------------------
	//				dx^2 + dy^2
	// where dx = x1 - x0, dy = y1 - y0
	// 
	float p0[2] = {0.4f * width,0.0f * height}, p1[2] = {0.6f * width,0.0f * height};
	
	
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
					stop0 = stops[0];
					for ( int i = 0; i < 4; i++ ) {
						finalcolor[i] = stop0[i+1];
					}

				} else if( g > 1 ) {
					stop0 = stops[stopCnt -1];
					for ( int i = 0; i < 4; i++ ) {
						finalcolor[i] = stop0[i+1];
					}

				} else {
					// determine which stops
					for ( int i = 0; i < stopCnt; i++ ) {
						if ( g >= stops[i][0] && g < stops[i+1][0] ) {
							stop0 = stops[i];
							stop1 = stops[i+1];
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
				int w = int(fabs(g));
				
				if ( spreadMode == VG_COLOR_RAMP_SPREAD_REPEAT ) {
					if ( g < 0 ) {
						g = 1 - (fabs(g) - w);
					} else {
						g = g - w;
					}
				} else if( spreadMode == VG_COLOR_RAMP_SPREAD_REFLECT ) {
					if ( g < 0 ) {
						if ( w % 2 == 0 ) { // even
							g = (fabs(g) - w);
						} else {	// odd
							g = (1 - (fabs(g) - w));
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
					if ( g >= stops[i][0] && g <= stops[i+1][0] ) {
						stop0 = stops[i];
						stop1 = stops[i+1];
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
	VGImage vgimage = vgCreateImage(VG_sRGBA_8888, width, height, 0 );
	
	vgImageSubData( vgimage, image, -1, VG_sRGBA_8888, 0, 0, width, height );
	
	free(image);
	
	return vgimage;

}

@implementation ES1Renderer



// Create an OpenGL ES 1.1 context
- (id)init
{
    if ((self = [super init]))
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }

        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
		
		// setup the OpenVG context
		vgCreateContextSH( 320, 480 );
		
		// create a paint
		_paint = vgCreatePaint();
		vgSetPaint(_paint, VG_FILL_PATH );
		
		VGfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		vgSetParameterfv(_paint, VG_PAINT_COLOR, 4, &color[0]);
		
		_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
		vguRect( _path, 50.0f, 50.0f, 90.0f, 50.0f );
		
		vgSetf( VG_STROKE_LINE_WIDTH, 7.0f );
		
		_image = [self buildVGImageFromUIImage:[UIImage imageNamed:@"zero.png"]];
		_bitmapFont = [self buildVGImageFromUIImage:[UIImage imageNamed:@"arial.png"]];
		
		_font = [self buildVGFontFromBitmapFont:@"arial"];

		_lineHeight = 74;	// hardwired.  todo: read from file
		
		// create a path for linear gradient
		_linearGradientPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
		vguRect( _linearGradientPath, 0, 0, 120.0f, 40.0f );
		
		// create a linear gradient paint to apply to the path
		_linearGradientPaint = vgCreatePaint();
		vgSetParameteri(_linearGradientPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
		
		// A linear gradient needs start and end points that describe orientation
		// and length of the gradient.
		float afLinearGradientPoints[4] = {
			0.0f, 0.0f,
			120.0f, 0.0f
		};
		vgSetParameterfv(_linearGradientPaint, VG_PAINT_LINEAR_GRADIENT, 4, afLinearGradientPoints);
		
		// Now we have to specify the colour ramp. It is described by "stops" that
		// are given as premultiplied sRGBA colour at a position between 0 and 1.
		// Between these stops, the colour is linearly interpolated.
		// This colour ramp goes from red to green to blue, all opaque.
		float stops[3][5] = {
			{0.0f,	1.0f, 0.0f, 0.0f, 1.0f},
			{0.5f,	0.0f, 1.0f, 0.0f, 0.8f},
			{1.0f,	0.0f, 0.0f, 1.0f, 0.4f}
		};
		vgSetParameterfv(_linearGradientPaint, VG_PAINT_COLOR_RAMP_STOPS, 15, &stops[0][0]);
		
		// setup radial gradient
		_radialGradientPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
		vguEllipse( _radialGradientPath, 0, 0, 90.0f, 50.0f );
		
		
		_radialGradientPaint = vgCreatePaint();
		vgSetParameteri(_radialGradientPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
		float afRadialGradient[5] = {45,25,45,25,45.0f };	// { cx, cy, fx, fy, r }.
		vgSetParameterfv(_radialGradientPaint, VG_PAINT_RADIAL_GRADIENT, 5, afRadialGradient);
		vgSetParameterfv(_radialGradientPaint, VG_PAINT_COLOR_RAMP_STOPS, 15, &stops[0][0]);
		//		loadTiger();
		//		
		//

    }

    return self;
}


- (void)render
{
    // This application only creates a single context which is already set current at this point.
    // This call is redundant, but needed if dealing with multiple contexts.
    [EAGLContext setCurrentContext:context];

    // This application only creates a single default framebuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	
	VGfloat clearColor[] = {1,1,1,1};
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	vgClear(0,0,backingWidth,backingHeight);
	
	/// draw the image
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL );
	vgLoadIdentity();
	vgTranslate( backingWidth/2, backingHeight/2 );
	vgDrawImage( _image );
	

	/// draw the basic path
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
	vgTranslate( backingWidth/2, backingHeight/2 );
	vgSetPaint( _paint, VG_FILL_PATH );
	vgDrawPath( _path, VG_FILL_PATH );

	/// draw the text
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
	vgLoadIdentity();
	VGfloat glyphOrigin[2] = {0,0};
	vgSetfv( VG_GLYPH_ORIGIN, 2, glyphOrigin );
	vgScale( 0.5f, 0.5f );
	vgTranslate( 10, backingHeight/2 );
	
	std::string s("Hello World! And puppies dogs?");
	vector<VGuint> glyphs;
	for( string::const_iterator it = s.begin(); it != s.end(); it++ )
		glyphs.push_back( VGuint( *it ) );

	// build the offset arrays
	size_t glyphCount = glyphs.size();
	vector<VGfloat> xadj;
	vector<VGfloat> yadj;
	for ( int i = 0; i < glyphCount; i++ ) {
		xadj.push_back( _glyphs[glyphs[i]].ox );
		yadj.push_back( _lineHeight - (_glyphs[glyphs[i]].height + _glyphs[glyphs[i]].oy) );
	}
	
	vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY );
	vgDrawGlyphs( _font, glyphCount, &glyphs[0], &xadj[0], &yadj[0], VG_FILL_PATH, VG_TRUE );

	// draw the gradient path
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
	vgTranslate( backingWidth/2, backingHeight/2 );
	vgSetPaint( _linearGradientPaint, VG_FILL_PATH );
	vgDrawPath( _linearGradientPath, VG_FILL_PATH );
	
	// draw radial gradient
	vgLoadIdentity();
	vgTranslate( 50, backingHeight/2 + 20 );
	vgSetPaint( _radialGradientPaint, VG_FILL_PATH );
	vgDrawPath( _radialGradientPath, VG_FILL_PATH );
	
    // This application only creates a single color renderbuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple renderbuffers.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{	
    // Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
    {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
	
	vgResizeSurfaceSH( backingWidth, backingHeight );

    return YES;
}


- (VGFont) buildVGFontFromBitmapFont:(NSString*)fontName {
	// load the bitmap image
	NSString* bitmapName = [fontName stringByAppendingPathExtension:@"png"];
	VGImage bitmapImage = [self buildVGImageFromUIImage:[UIImage imageNamed:bitmapName]];

	
	NSString* fontDescriptionPath = [[NSBundle mainBundle] pathForResource:fontName ofType:@"fnt"];
	
	// create openvg font
	VGFont font = vgCreateFont( 0 );
	
	// Read the contents of the file into a string
	NSString *contents = [NSString stringWithContentsOfFile:fontDescriptionPath encoding:NSASCIIStringEncoding error:nil];
	
	// Move all lines in the string, which are denoted by \n, into an array
	NSArray *lines = [[NSArray alloc] initWithArray:[contents componentsSeparatedByString:@"\n"]];
	
	// Create an enumerator which we can use to move through the lines read from the control file
	NSEnumerator *nse = [lines objectEnumerator];
	
	// Create a holder for each line we are going to work with
	NSString *line;
	
	
	// Loop through all the lines in the lines array processing each one
	while(line = [nse nextObject]) {
		// Check to see if the start of the line is something we are interested in
		if([line hasPrefix:@"common"]) {
			//[self parseCommon:line];  //// NEW CODE ADDED 05/02/10 to parse the common params
		} else if([line hasPrefix:@"char"]) {
			// Break the values for this line up using =
			NSArray *values = [line componentsSeparatedByString:@"="];
			if( [values count] < 3 )
				continue;
			
			// Get the enumerator for the array of components which has been created
			NSEnumerator *charNse = [values objectEnumerator];
			
			// We are going to place each value we read from the line into this string
			NSString *propertyValue;
			
			// We need to move past the first entry in the array before we start assigning values
			[charNse nextObject];
			
//			VG_API_CALL void VG_API_ENTRY vgSetGlyphToImage(VGFont font,
//															VGuint glyphIndex,
//															VGImage image,
//															VGfloat glyphOrigin [2],
//															VGfloat escapement[2]) VG_API_EXIT {

			VGuint glyphIndex = -1;
			VGfloat glyphOrigin[2] = {0,0};
			VGfloat escapement[2] = {0,0};
			VGfloat size[2] = {0,0}; 
			GlyphDescription glyph;
			
			// Character ID
			propertyValue = [charNse nextObject];
			glyphIndex = [propertyValue intValue]; 
			// Character x
			propertyValue = [charNse nextObject];
			glyphOrigin[0] = [propertyValue intValue];
			// Character y
			propertyValue = [charNse nextObject];
			glyphOrigin[1] = [propertyValue intValue];
			// Character width
			propertyValue = [charNse nextObject];
			size[0] = [propertyValue intValue];
			// Character height
			propertyValue = [charNse nextObject];
			glyph.height = size[1] = [propertyValue intValue];
			// Character xoffset
			propertyValue = [charNse nextObject];
			glyph.ox = [propertyValue intValue];
			// Character yoffset
			propertyValue = [charNse nextObject];
			glyph.oy = [propertyValue intValue];
			// Character xadvance
			propertyValue = [charNse nextObject];
			escapement[0] = [propertyValue intValue];
			
			
			// creaet a sub child image from the bitmap font image
			
			glyph.image = vgChildImage(bitmapImage, glyphOrigin[0], glyphOrigin[1], size[0], size[1] );
			
			// store away for later use
			_glyphs[glyphIndex] = glyph;
			// add the glyph to the font
			vgSetGlyphToImage( font, glyphIndex, glyph.image, glyphOrigin, escapement );
		}		
	}
	// Finished with lines so release it
	[lines release];
	
	return font;

}


#define kMaxTextureSize	 1024
- (VGImage) buildVGImageFromUIImage:(UIImage *)uiImage {
	NSUInteger				width,
	height,
	i;
	CGContextRef			ctx = nil;
	void*					data = nil;
	CGColorSpaceRef			colorSpace;
	void*					tempData;
	unsigned int*			inPixel32;
	unsigned short*			outPixel16;
	BOOL					hasAlpha;
	CGImageAlphaInfo		info;
	CGAffineTransform		transform;
	CGSize					imageSize;
	VGImageFormat			pixelFormat;
	CGImageRef				image;
	UIImageOrientation		orientation;
	BOOL					sizeToFit = NO;
	
	
	image = [uiImage CGImage];
	orientation = [uiImage imageOrientation]; 
	
	if(image == NULL) {
		[self release];
		NSLog(@"Image is Null");
		return VG_INVALID_HANDLE;
	}
	
	
	info = CGImageGetAlphaInfo(image);
	hasAlpha = ((info == kCGImageAlphaPremultipliedLast) || (info == kCGImageAlphaPremultipliedFirst) || (info == kCGImageAlphaLast) || (info == kCGImageAlphaFirst) ? YES : NO);
	if(CGImageGetColorSpace(image)) {
		if(hasAlpha)
			pixelFormat = VG_sRGBA_8888;
		else
			pixelFormat = VG_sRGB_565;
	} else  //NOTE: No colorspace means a mask image
		pixelFormat = VG_A_8;
	
	
	imageSize = CGSizeMake(CGImageGetWidth(image), CGImageGetHeight(image));
	transform = CGAffineTransformIdentity;
	
	width = imageSize.width;
	
	if((width != 1) && (width & (width - 1))) {
		i = 1;
		while((sizeToFit ? 2 * i : i) < width)
			i *= 2;
		width = i;
	}
	height = imageSize.height;
	if((height != 1) && (height & (height - 1))) {
		i = 1;
		while((sizeToFit ? 2 * i : i) < height)
			i *= 2;
		height = i;
	}
	while((width > kMaxTextureSize) || (height > kMaxTextureSize)) {
		width /= 2;
		height /= 2;
		transform = CGAffineTransformScale(transform, 0.5, 0.5);
		imageSize.width *= 0.5;
		imageSize.height *= 0.5;
	}
	
	//colorSpace = CGImageGetColorSpace(image);
	
	switch(pixelFormat) {		
		case VG_sRGBA_8888:
			colorSpace = CGColorSpaceCreateDeviceRGB();
			data = malloc(height * width * 4);
			ctx = CGBitmapContextCreate(data, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
			CGColorSpaceRelease(colorSpace);
			break;
		case VG_sRGB_565:
			colorSpace = CGColorSpaceCreateDeviceRGB();
			data = malloc(height * width * 4);
			ctx = CGBitmapContextCreate(data, width, height, 8, 4 * width, colorSpace, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big);
			CGColorSpaceRelease(colorSpace);
			break;
			
		case VG_A_8:
			data = malloc(height * width);
			ctx = CGBitmapContextCreate(data, width, height, 8, width, NULL, kCGImageAlphaOnly);
			break;				
		default:
			[NSException raise:NSInternalInconsistencyException format:@"Invalid pixel format"];
	}
	
	
	CGContextClearRect(ctx, CGRectMake(0, 0, width, height));
	CGContextTranslateCTM(ctx, 0, height - imageSize.height);
	
	if(!CGAffineTransformIsIdentity(transform))
		CGContextConcatCTM(ctx, transform);
	CGContextDrawImage(ctx, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)), image);
	//Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGGBBBBB"
	if(pixelFormat == VG_sRGB_565) {
		tempData = malloc(height * width * 2);
		inPixel32 = (unsigned int*)data;
		outPixel16 = (unsigned short*)tempData;
		for(i = 0; i < width * height; ++i, ++inPixel32)
			*outPixel16++ = ((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) | ((((*inPixel32 >> 8) & 0xFF) >> 2) << 5) | ((((*inPixel32 >> 16) & 0xFF) >> 3) << 0);
		free(data);
		data = tempData;
		
	}
	
	// create openvg image
	VGImage vgimage = vgCreateImage(pixelFormat, width, height, 0 );
	
	vgImageSubData( vgimage, data, -1, pixelFormat, 0, 0, width, height );
	
	CGContextRelease(ctx);
	free(data);
	
	return vgimage;
}


- (void)dealloc
{
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];
    context = nil;

    [super dealloc];
}

@end
