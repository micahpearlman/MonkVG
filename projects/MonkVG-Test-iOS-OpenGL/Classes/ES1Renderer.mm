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
#include "svg.h"


extern "C" void loadTiger();
extern "C" void display(float dt);

using namespace std;
SVGHandler* _handler;



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
		vguEllipse( _path, 100.0f, 100.0f, 90.0f, 50.0f );
		
		vgSetf( VG_STROKE_LINE_WIDTH, 7.0f );
		
//		loadTiger();
//		
//
//		MonkSVG::SVG svg_parser;
//		_handler = new SVGHandler;
//		
//		svg_parser.initialize( _handler );
//		
//		NSString *base_path = [[NSBundle mainBundle] resourcePath];
//		std::string path = std::string( [base_path UTF8String] ) + string("/circle_poly.svg");
//		
//		fstream file( path.c_str() );
//		if ( file.is_open() ) {
//			std::string line;
//			std::string buf;
//			while( std::getline( file, line) )
//				buf += line;
//			std::cout << "read: " << buf << "\n";
//			svg_parser.read( buf );
//		}
		
		
    }

    return self;
}


- (void)render
{
	static float ang = 0;
    // This application only creates a single context which is already set current at this point.
    // This call is redundant, but needed if dealing with multiple contexts.
    [EAGLContext setCurrentContext:context];

    // This application only creates a single default framebuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

//	_handler->draw();
//	display(0.05f);
	
	VGfloat clearColor[] = {1,1,1,1};
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	vgClear(0,0,320,480);
	
	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
//	vgTranslate(320/2,(480/2)/2);
//	vgRotate(ang);
//	ang += 0.5f;
	
	vgDrawPath( _path, VG_FILL_PATH );

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

    return YES;
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
