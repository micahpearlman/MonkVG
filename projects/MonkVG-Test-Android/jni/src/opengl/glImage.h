//
//  glImage.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __glImage_h__
#define __glImage_h__

#include "mkImage.h"

#if defined(__APPLE__)
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #include "glu.h"
    #else
        #include <OpenGL/glu.h>
        #define glOrthof glOrtho
    #endif
#else // #if defined(__APPLE__)
    #if ANDROID
        #include "glu.h"
    #endif
#endif // #if defined(__APPLE__)

namespace MonkVG {
	class OpenGLImage : public IImage {
	public:
		
		OpenGLImage( VGImageFormat format,
					VGint width, VGint height,
					VGbitfield allowedQuality );
		OpenGLImage( OpenGLImage& other );

		virtual ~OpenGLImage();
		
		virtual IImage* createChild( VGint x, VGint y, VGint w, VGint h );
		
		// drawing
		virtual void draw();
		virtual void drawSubRect( VGint ox, VGint oy, VGint w, VGint h, VGbitfield paintModes );
		virtual void drawToRect( VGint x, VGint y, VGint w, VGint h, VGbitfield paintModes );
		virtual void drawAtPoint( VGint x, VGint y, VGbitfield paintModes );
		
		virtual void setSubData( const void * data, VGint dataStride,
								VGImageFormat dataFormat,
								VGint x, VGint y, VGint width, VGint height );

	private:
		GLuint						_name;
		
	};
}

#endif // __glImage_h__
