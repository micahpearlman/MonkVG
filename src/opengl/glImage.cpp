//
//  glImage.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include "glImage.h"
#include "glContext.h"

namespace MonkVG {
	
	OpenGLImage::OpenGLImage( VGImageFormat format,
								VGint width, VGint height,
								VGbitfield allowedQuality )
	:	IImage(format, width, height, allowedQuality )
	,	_name( 0 )
	{
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &_name);
		glBindTexture(GL_TEXTURE_2D, _name);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		//		/* RGB{A,X} channel ordering */
		//		VG_sRGBX_8888                               =  0,
		//		VG_sRGBA_8888                               =  1,
		//		VG_sRGBA_8888_PRE                           =  2,
		//		VG_sRGB_565                                 =  3,
		//		VG_sRGBA_5551                               =  4,
		//		VG_sRGBA_4444                               =  5,
		//		VG_sL_8                                     =  6,
		//		VG_lRGBX_8888                               =  7,
		//		VG_lRGBA_8888                               =  8,
		//		VG_lRGBA_8888_PRE                           =  9,
		//		VG_lL_8                                     = 10,
		//		VG_A_8                                      = 11,
		//		VG_BW_1                                     = 12,
		//		VG_A_1                                      = 13,
		//		VG_A_4                                      = 14,

		switch(format) {
				
			case VG_sRGBA_8888:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				break;
			case VG_sRGB_565:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);
				break;
			case VG_A_8:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
				break;
			default:
				IContext::instance().setError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
				assert(0);
				break;
				
		}
	}
	
	OpenGLImage::~OpenGLImage() {
		if( _name ) {
			glDeleteTextures( 1, &_name );
			_name = 0;
		}
	}
	
	
	void OpenGLImage::setSubData( const void * data, VGint dataStride,
								 VGImageFormat dataFormat,
								 VGint x, VGint y, VGint width, VGint height ) {
		assert( _name );
		
		glBindTexture(GL_TEXTURE_2D, _name);
		
		switch(dataFormat) {
				
			case VG_sRGBA_8888:
				glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data );
				break;
			case VG_sRGB_565:
				glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data );
				break;
			case VG_A_8:
				glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, data );
				break;
			default:
				IContext::instance().setError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
				assert(0);
				break;
				
		}

		
	}

	void OpenGLImage::draw() {
		GLfloat		coordinates[] = {	0,	1,
										1,	1,
										0,	0,
										1,	0 };
		GLfloat	width = (GLfloat)_width;
		GLfloat height = (GLfloat)_height;
		GLfloat		vertices[] = {	-width / 2,	-height / 2,	0.0,
									width / 2,	-height / 2,	0.0,
									-width / 2,	height / 2,		0.0,
									width / 2,	height / 2,		0.0 };
		
		glEnable(GL_TEXTURE_2D);
		// turn on blending
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_SRC_COLOR);	

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState( GL_COLOR_ARRAY );
		
		glDisable( GL_CULL_FACE );
		

		
		glBindTexture(GL_TEXTURE_2D, _name);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void OpenGLImage::drawSubRect( VGint ox, VGint oy, VGint w, VGint h, VGbitfield paintModes ) {
		GLfloat minS = GLfloat(ox) / GLfloat(_width);
		GLfloat maxS = GLfloat(ox + w) / GLfloat(_width);
		GLfloat minT = GLfloat(oy) / GLfloat(_width);
		GLfloat maxT = GLfloat(oy + h) / GLfloat(_width);
		
		GLfloat		coordinates[] = {	minS, maxT,		//0,	1,
										maxS, maxT,		//1,	1,
										minS, minT,		//0,	0,
										maxS, minT };	//1,	0 
		GLfloat	width = (GLfloat)w;
		GLfloat height = (GLfloat)h;
		GLfloat		vertices[] = {	-width / 2,	-height / 2,	0.0,
			width / 2,	-height / 2,	0.0,
			-width / 2,	height / 2,		0.0,
			width / 2,	height / 2,		0.0 };
		
		glEnable(GL_TEXTURE_2D);
		// turn on blending
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_SRC_COLOR);	
		
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState( GL_COLOR_ARRAY );
		
		glDisable( GL_CULL_FACE );
		
		
		
		glBindTexture(GL_TEXTURE_2D, _name);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
	}
}