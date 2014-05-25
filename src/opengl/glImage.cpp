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
		GL->glEnable(GL_TEXTURE_2D);
		GL->glGenTextures(1, &_name);
		GL->glBindTexture(GL_TEXTURE_2D, _name);
		GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GL->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
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
				GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				break;
			case VG_sRGB_565:
				GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);
				break;
			case VG_A_8:
				GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
				break;
			default:
				SetError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
				assert(0);
				break;
				
		}
	}
	
	OpenGLImage::OpenGLImage( OpenGLImage& other ) 
	:	IImage( other )
	,	_name( other._name )
	{
		
	}
	
	OpenGLImage::~OpenGLImage() {
		if( !_parent && _name ) {
			GL->glDeleteTextures( 1, &_name );
			_name = 0;
		}
	}
	
	IImage* OpenGLImage::createChild( VGint x, VGint y, VGint w, VGint h ) {
		OpenGLImage* glImage = new OpenGLImage( *this );
		glImage->_s[0] = VGfloat(x)/VGfloat(_width);
		glImage->_s[1] = VGfloat(x+w)/VGfloat(_width);
		glImage->_t[0] = VGfloat(y)/VGfloat(_height);
		glImage->_t[1] = VGfloat(y+h)/VGfloat(_height);
		glImage->_width = w;
		glImage->_height = h;
		
		return glImage;
	}
	
	
	void OpenGLImage::setSubData( const void * data, VGint dataStride,
								 VGImageFormat dataFormat,
								 VGint x, VGint y, VGint width, VGint height ) {
		assert( _name );
		
		GL->glBindTexture(GL_TEXTURE_2D, _name);
		
		switch(dataFormat) {
				
			case VG_sRGBA_8888:
				GL->glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data );
				break;
			case VG_sRGB_565:
				GL->glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data );
				break;
			case VG_A_8:
				GL->glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, data );
				break;
			default:
				SetError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
				assert(0);
				break;
				
		}

		
	}

	void OpenGLImage::draw() {
		
		
		GLfloat		coordinates[] = {	_s[0],	_t[1],
										_s[1],	_t[1],
										_s[0],	_t[0],
										_s[1],	_t[0] };
		
		GLfloat	w = (GLfloat)_width;
		GLfloat h = (GLfloat)_height;
		GLfloat x = 0, y = 0;
		// note openvg coordinate system is bottom, left is 0,0
		GLfloat		vertices[] = 
		{	x,		y,		0.0f,	// left, bottom
			x+w,	y,		0.0f,	// right, bottom
			x,		y+h,	0.0f,	// left, top
			x+w,	y+h,	0.0f };	// right, top
		
		
		if ( IContext::instance().getImageMode() == VG_DRAW_IMAGE_MULTIPLY ) {
			// set the color to the current fill paint color
			IPaint* fillPaint = IContext::instance().getFillPaint();
			const VGfloat* color = fillPaint->getPaintColor();
			GL->glColor4f( color[0], color[1], color[2], color[3] );
		}

		bind();
//		glEnable(GL_TEXTURE_2D);
//		// turn on blending
//		glEnable(GL_BLEND);
//		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
//		glBindTexture(GL_TEXTURE_2D, _name);

		
		GL->glEnableClientState(GL_VERTEX_ARRAY);
		GL->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		GL->glDisableClientState( GL_COLOR_ARRAY );
		
		GL->glDisable( GL_CULL_FACE );
		

		
		
		GL->glVertexPointer(3, GL_FLOAT, 0, vertices);
		GL->glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		GL->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		GL->glBindTexture(GL_TEXTURE_2D, 0);

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

		GLfloat x = 0, y = 0;
		// note openvg coordinate system is bottom, left is 0,0
		GLfloat		vertices[] = 
		{	x,		y,		0.0f,	// left, bottom
			x+w,	y,		0.0f,	// right, bottom
			x,		y+h,	0.0f,	// left, top
			x+w,	y+h,	0.0f };	// right, top
		
		if ( IContext::instance().getImageMode() == VG_DRAW_IMAGE_MULTIPLY ) {
			// set the color to the current fill paint color
			IPaint* fillPaint = IContext::instance().getFillPaint();
			const VGfloat* color = fillPaint->getPaintColor();
			GL->glColor4f( color[0], color[1], color[2], color[3] );
		}

		
		GL->glEnable(GL_TEXTURE_2D);
		// turn on blending
		GL->glEnable(GL_BLEND);
		GL->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		
		GL->glEnableClientState(GL_VERTEX_ARRAY);
		GL->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		GL->glDisableClientState( GL_COLOR_ARRAY );
		
		GL->glDisable( GL_CULL_FACE );
		
		
		
		GL->glBindTexture(GL_TEXTURE_2D, _name);
		GL->glVertexPointer(3, GL_FLOAT, 0, vertices);
		GL->glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		GL->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		GL->glBindTexture(GL_TEXTURE_2D, 0);

		
	}
	
	void OpenGLImage::drawToRect( VGint x, VGint y, VGint w, VGint h, VGbitfield paintModes ) {
		GLfloat	coordinates[] = {	_s[0],	_t[1],
			_s[1],	_t[1],
			_s[0],	_t[0],
			_s[1],	_t[0] };
		// note openvg coordinate system is bottom, left is 0,0
		GLfloat		vertices[] = 
		{	(GLfloat)x,		(GLfloat)y,		0.0f,	// left, bottom
			(GLfloat)(x+w),	(GLfloat)y,		0.0f,	// right, bottom
			(GLfloat)x,		(GLfloat)(y+h),	0.0f,	// left, top
			(GLfloat)(x+w),	(GLfloat)(y+h),	0.0f };	// right, top
		
		if ( IContext::instance().getImageMode() == VG_DRAW_IMAGE_MULTIPLY ) {
			// set the color to the current fill paint color
			IPaint* fillPaint = IContext::instance().getFillPaint();
			const VGfloat* color = fillPaint->getPaintColor();
			glColor4f( color[0], color[1], color[2], color[3] );
		}

		
		GL->glEnable(GL_TEXTURE_2D);
		// turn on blending
		GL->glEnable(GL_BLEND);
		GL->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		
		GL->glEnableClientState(GL_VERTEX_ARRAY);
		GL->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		GL->glDisableClientState( GL_COLOR_ARRAY );
		
		GL->glDisable( GL_CULL_FACE );
		
		
		
		GL->glBindTexture(GL_TEXTURE_2D, _name);
		GL->glVertexPointer(3, GL_FLOAT, 0, vertices);
		GL->glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		GL->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		GL->glBindTexture(GL_TEXTURE_2D, 0);

	}
	
	void OpenGLImage::drawAtPoint( VGint x, VGint y, VGbitfield paintModes ) {
		drawToRect( x, y, _width, _height, paintModes );
	}
	
	void OpenGLImage::bind() {
		GL->glEnable(GL_TEXTURE_2D);
		// turn on blending
		GL->glEnable(GL_BLEND);
		GL->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		
		GL->glBindTexture(GL_TEXTURE_2D, _name);

	}
	void OpenGLImage::unbind() {
		GL->glBindTexture(GL_TEXTURE_2D, 0);
	}

}
