MonkVG: An OpenVG implementation
==================================
![Tiger](tiger.png)

## Overview

MonkVG is an OpenVG 1.1 *like* vector graphics API implementation currently using an OpenGL ES (1.1 or 2.0) backend that should be compatible with any HW that supports OpenGL ES 1.1 or 2.0, which includes most iOS and Android devices as well as Linux, MacOS and Windows platforms. 

This is an open source BSD licensed project that is in active development. Contributors and sponsors welcome.

It can be found at GitHub http://github.com/micahpearlman/MonkVG

Projects using MonkVG include:

- MonkSVG: A SVG parser and renderer.  https://github.com/micahpearlman/MonkSVG
- CCSVG: A SVG renderer for Cocos2D. https://github.com/lukelutman/CCSVG

## Whats New

- (2/17/2022) CMake build support and GLFW example.
- (1/22/2012) Now supports OpenGL ES 1.1 *AND* 2.0

## Installation

Use git to clone:  

```
git clone --recursive https://github.com/micahpearlman/MonkVG.git
```

There are currently iOS and OSX XCode 4 projects as well as contributed Android projects (thanks Paul Holden)and Windows project (thanks Vincent Richomme).
### CMake Build

```
mkdir ./build
cd ./build

cmake --build .

# run the GLFW example
./glfw_hello_world 
```
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

## Extensions

MonkVG was originally created for games, so speed has usually been prefered over quality or full OpenVG compliance.  To improve speed there have been MonkVG specific extensions. See "vgext.h" for some of the details.

## Contributors

Paul Holden (Initial Android Port)  
Vincent Richomme (Windows Port)  
Gav Wood (Android and Linux Port) 

Also Luke contributed a great article on how to integrate MonkVG + MonkSWF with Cocos2D: http://blog.zincroe.com/2011/11/displaying-a-swf-on-the-iphone-with-cocos2d-and-monkswf/

## Simple Example

See `./examples` directory.

NOTE:  MonkVG will not create a OpenGL context, it is the applications responsibility to create there own OpenGL context.
Also, if your application does any other OpenGL rendering it should save off the GL state and then restore before calling any MonkVG methods.

```
	
VGPaint _paint;
VGPath _path;
void init() {
	... setup platform specific opengl ...
	// setup the OpenVG context
	vgCreateContextMNK( 320, 480, VG_RENDERING_BACKEND_TYPE_OPENGLES20 );

	...OR... for OpenGL ES 1.1

	vgCreateContextMNK( 320, 480, VG_RENDERING_BACKEND_TYPE_OPENGLES11 );

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
	... save any GL state here ...
	... start opengl context ...

	/// draw the basic path
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
	vgTranslate( screenWidth/2, screenHeight/2 );
	vgSetPaint( _paint, VG_FILL_PATH );
	vgDrawPath( _path, VG_FILL_PATH );

	... end opengl context ...
	... restore and GL state here ...
}
```


## Android Build instructions

### Overview

Tested with ndk r9d on ubuntu.

There are 2 projects needed to build the example app: 

1. MonkVG-Android builds the needed MonkVG libraries

2. MonkVG-Test-Android wraps them and makes them accessible from an activity.

### Build steps

```bash
ndk-build V=1
```
The `V=1` makes the output verbose.
I used android-17 but any of the available targets should do.
If ndk-build fails it may be helpful to import the projects in Eclipse and add the Native Tools (Right click on project name->**Android Tools-> Add Native Support**)
make sure to `rm -rf obj/local/armeabi` on both projects for rebuilding, in order not to use old builds of the static libraries.

### Deploy on device
```bash
android list targets
android update project -p . --target android-17
ant debug 
ant debug install
ant release install
```
If you imported the projects in Eclipse right click->Run As-> Android App will do.
