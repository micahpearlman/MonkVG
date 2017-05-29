//
//  MonkVGExample.m
//  MonkVG-Test-iOS-OpenGL
//
//  Created by Micah Pearlman on 1/19/13.
//
//

#import "MonkVGExample.h"
#include <MonkVG/openvg.h>
#include <MonkVG/vgu.h>
#include <MonkVG/vgext.h>
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


@interface MonkVGExample () {
    VGPath	_path;
	VGPaint _paint;
	VGPaint _linearGradientPaint;
	VGPaint _radialGradientPaint;
	VGImage _image;
	VGImage	_bitmapFont;
	//	VGImage _gradientImage;
	VGPath _linearGradientPath;
	VGPath _radialGradientPath;
	VGFont	_font;
	VGfloat _lineHeight;
    
}

- (VGImage) buildVGImageFromUIImage:(UIImage *)uiImage;
- (VGFont) buildVGFontFromBitmapFont:(NSString*)fontName;


@end

@implementation MonkVGExample

- (id) init {
    if ( self = [super init]) {
        
        
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
		
        loadTiger();

    }
    
    return self;
}

- (void) render {
    
    // get the width and height of the screen
    VGint backingWidth = vgGeti( VG_SURFACE_WIDTH_MNK );
    VGint backingHeight = vgGeti( VG_SURFACE_HEIGHT_MNK );;
    
    
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
    
    // draw tiger
    display( 1.0f/20.0f );

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


@end
