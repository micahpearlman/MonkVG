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
		
		vgCreateContextSH( 320, 480 );
		
		_paint = vgCreatePaint();
		vgSetPaint(_paint, VG_FILL_PATH );
		
		VGfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		vgSetParameterfv(_paint, VG_PAINT_COLOR, 4, &color[0]);
		
		_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
		//vguRect( _path, 100.0f, 100.0f, 90.0f, 50.0f );
		vguEllipse( _path, 0, 0, 90.0f, 50.0f );
		
		vgSetf( VG_STROKE_LINE_WIDTH, 7.0f );
		
		_image = [self buildVGImageFromUIImage:[UIImage imageNamed:@"zero.png"]];
		_bitmapFont = [self buildVGImageFromUIImage:[UIImage imageNamed:@"arial.png"]];
		
		_font = [self buildVGFontFromBitmapFont:@"arial"];

		_lineHeight = 74;	// hardwired.  todo: read from file
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
	
	
	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgLoadIdentity();
	vgTranslate( backingWidth/2, backingHeight/2 );
	vgDrawImage( _image );

	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
	vgTranslate( backingWidth/2, backingHeight/2 );
	vgDrawPath( _path, VG_FILL_PATH );

	vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
	vgLoadIdentity();
	VGfloat glyphOrigin[2] = {0,0};
	vgSetfv( VG_GLYPH_ORIGIN, 2, glyphOrigin );
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
	

	vgDrawGlyphs( _font, glyphCount, &glyphs[0], &xadj[0], &yadj[0], VG_FILL_PATH, VG_TRUE );
//	vgDrawGlyph( _font, int('Z'), VG_FILL_PATH, VG_TRUE );
//	vgDrawGlyph( _font, int('e'), VG_FILL_PATH, VG_TRUE );
//	vgDrawGlyph( _font, int('r'), VG_FILL_PATH, VG_TRUE );
//	vgDrawGlyph( _font, int('o'), VG_FILL_PATH, VG_TRUE );

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
