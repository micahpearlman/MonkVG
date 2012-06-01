//
//  ES1Renderer.h
//  MonkVG-Test-iOS-OpenGL
//
//  Created by Micah Pearlman on 8/11/10.
//  Copyright Zero Vision 2010. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include <MonkVG/openvg.h>
#include <MonkVG/vgu.h>

//class SVGHandler;

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer, colorRenderbuffer;
	
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

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;
- (VGImage) buildVGImageFromUIImage:(UIImage *)uiImage;
- (VGFont) buildVGFontFromBitmapFont:(NSString*)fontName;

@end
