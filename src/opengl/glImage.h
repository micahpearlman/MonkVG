//
//  glImage.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#include "mkImage.h"
#if defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#include "glu.h"
#else
#include <OpenGL/glu.h>
#define glOrthof glOrtho
#endif
#endif // #if defined(__APPLE__)

namespace MonkVG {
	class OpenGLImage : public IImage {
	public:
		
		OpenGLImage( VGImageFormat format,
					VGint width, VGint height,
					VGbitfield allowedQuality );

		virtual ~OpenGLImage();
		
		virtual void draw();
		
		virtual void setSubData( const void * data, VGint dataStride,
								VGImageFormat dataFormat,
								VGint x, VGint y, VGint width, VGint height );

	private:
		GLuint						_name;
		
	};
}