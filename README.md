MonkVG -- An OpenVG implementation
==================================

## Overview

MonkVG is an OpenVG 1.1 *like* vector graphics API implementation currently using an OpenGL ES backend that should be compatible with any HW that supports OpenGL ES 1.1 which includes most iOS and Android devices as well as OSX and Windows platforms. 

This is an open source BSD licensed project that is in active development. Contributors and sponsors welcome.

It can be found at GitHub http://github.com/micahpearlman/MonkVG

Projects using MonkVG include:

- MonkSWF: A flash swf parser and renderer. https://github.com/micahpearlman/MonkSWF
- MonkSVG: A SVG parser and renderer.  https://github.com/micahpearlman/MonkSVG

## Installation

Not even versioned yet for download.  Use git to clone:  

<tt>$ git clone git@github.com:micahpearlman/MonkVG.git</tt>

There are currently iOS and OSX XCode 4 projects as well as contributed Android projects (thanks Paul Holden)and Windows project (thanks Vincent Richomme).

TODO: standard *nix Makefiles.


## Extensions

MonkVG was originally created for games, so speed has usually been prefered over quality or full OpenVG compliance.  To improve speed there have been MonkVG specific extensions. See "vgext.h" for some of the details.

## What is implemented

- Most all path segment commands including: moves, lines, bezier curves, elliptical arcs.
- Robust contour tesselation supporting both fill rules.
- Very basic stroking.
- Most paints including: Solid color fill, linear and radial gradients.  
- Bitmap font rendering.
- Bitmap image rendering.
- OpenVG utility library (see vgu.h)
	
## TODO
- Pattern fills and strokes.
- Gradient fills on strokes (works for fills).
- Stroke font rendering (may work just untested).
- Various blending modes (somewhat working already).
- Scissoring and masking.
- Improve stroking geometry generation.

## Probably never support
- Image filters.
- Anti-aliasing. (Though this can be supported outside OpenVG.  For example iOS fullscreen AA glRenderbufferStorageMultisampleAPPLE method)

## Contributors

Paul Holden (Android Port)  
Vincent Richomme (Windows Port)  

Also Luke contributed a great article on how to integrate MonkVG + MonkSWF with Cocos2D: http://blog.zincroe.com/2011/11/displaying-a-swf-on-the-iphone-with-cocos2d-and-monkswf/

## Simple Example

NOTE:  MonkVG will not create a OpenGL context, it is the applications responsibility to create there own OpenGL context.
Also, if your application does any other OpenGL rendering it should save off the GL state and then restore before calling any MonkVG methods.

<tt>
	
	VGPaint _paint;
	VGPath _path;
	void init() {
		... setup platform specific opengl ...
		// setup the OpenVG context
		vgCreateContextSH( 320, 480 );
		
		// create a paint
		_paint = vgCreatePaint();
		vgSetPaint(_paint, VG_FILL_PATH );
		VGfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		vgSetParameterfv(_paint, VG_PAINT_COLOR, 4, &color[0]);
		
		// create a box path
		_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
		vguRect( _path, 50.0f, 50.0f, 90.0f, 50.0f );
		
		
	}
	
	void draw() {
		... start opengl context ...
		/// draw the basic path
		vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
		vgLoadIdentity();
		vgTranslate( screenWidth/2, screenHeight/2 );
		vgSetPaint( _paint, VG_FILL_PATH );
		vgDrawPath( _path, VG_FILL_PATH );
		
		... end opengl context ...
	}
</tt>

