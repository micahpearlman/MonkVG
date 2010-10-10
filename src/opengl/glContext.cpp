/*
 *  glContext.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glContext.h"
#include "glPath.h"
#include "glPaint.h"
#include "mkCommon.h"

namespace MonkVG {
	
	OpenGLContext::OpenGLContext()
		:	IContext()
	{
	}
	
	void OpenGLContext::checkGLError() {
		
		int err = glGetError();
		
		
		const char* RVAL = "GL_UNKNOWN_ERROR";
		
		switch( err )
		{
			case GL_NO_ERROR:
				RVAL =  "GL_NO_ERROR";
				break;
			case GL_INVALID_ENUM:
				RVAL =  "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				RVAL =  "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				RVAL = "GL_INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				RVAL =  "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				RVAL =  "GL_STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				RVAL =  "GL_OUT_OF_MEMORY";
				break;
			default:
				break;
		}
		
		if( err != GL_NO_ERROR ) {
			printf("GL_ERROR: %s\n", RVAL );
			MK_ASSERT(0);
		}
	}	
	
	bool OpenGLContext::Initialize() {
	
//		/* setup GL projection */
//		glViewport(0,0, _width, _height);
//		
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		glOrthof(0, _width, _height, 0, -1, 1);
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
		
		return true;
	}
	
	
	bool OpenGLContext::Terminate() {
		return true;
	}
	
	
	void OpenGLContext::beginRender() {
		
		CHECK_GL_ERROR;
		
		// get viewport to restore back when we are done
		glGetIntegerv( GL_VIEWPORT, _viewport );
		glGetFloatv( GL_PROJECTION_MATRIX, _projection );
		glGetFloatv( GL_MODELVIEW_MATRIX, _modelview );
		
		// get the color to back up when we are done
		glGetFloatv( GL_CURRENT_COLOR, _color );
		
		// setup GL projection 
		glViewport(0,0, _width, _height);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(0, _width, _height, 0, -1, 1);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glDisable( GL_CULL_FACE );
		
		// turn on blending
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		
		CHECK_GL_ERROR;
		
	}
	void OpenGLContext::endRender() {
		
		CHECK_GL_ERROR;
		
		// todo: restore state to be nice to other apps
		// restore the old viewport
		glMatrixMode( GL_PROJECTION );
		glLoadMatrixf( _projection );
		glViewport( _viewport[0], _viewport[1], _viewport[2], _viewport[3] );
		glMatrixMode( GL_MODELVIEW );			
		glLoadMatrixf( _modelview );
		
		// restore color
		glColor4f( _color[0], _color[1], _color[2], _color[3] );
		
		CHECK_GL_ERROR;
	}

	
	
	IPath* OpenGLContext::createPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) {
	
		OpenGLPath *path = new OpenGLPath(pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities  &= VG_PATH_CAPABILITY_ALL);
		if( path == 0 )
			setError( VG_OUT_OF_MEMORY_ERROR );
	
		return (IPath*)path;
	}
	
	void OpenGLContext::destroyPath( IPath* path ) {
		delete (OpenGLPath*)path;
	}
	
	IPaint* OpenGLContext::createPaint() {
		OpenGLPaint *paint = new OpenGLPaint();
		if( paint == 0 )
			setError( VG_OUT_OF_MEMORY_ERROR );
		return (IPaint*)paint;
	}
	
	void OpenGLContext::stroke() {
		if ( _stroke_paint ) {
			const VGfloat* c = _stroke_paint->getPaintColor();
			glColor4f(c[0], c[1], c[2], c[3] );
		}
	}
	
	void OpenGLContext::fill() {
		if ( _fill_paint ) {
			const VGfloat* c = _fill_paint->getPaintColor();
			glColor4f(c[0], c[1], c[2], c[3] );
		}
		
	}
	
	void OpenGLContext::clear(VGint x, VGint y, VGint width, VGint height) {
	}
	
	void OpenGLContext::setIdentity() {
		Matrix33* active = getActiveMatrix();
		active->setIdentity();
	}
	void OpenGLContext::transform() {
		// a	b	0
		// c	d	0
		// tx	ty	1
//		Matrix33 active = *getActiveMatrix();
//		active.transpose();
//		
	}

	void OpenGLContext::scale( VGfloat sx, VGfloat sy ) {
		Matrix33* active = getActiveMatrix();
		Matrix33 scale;
		scale.setScale( sx, sy );
		active->multiply( scale );
	}
	void OpenGLContext::translate( VGfloat x, VGfloat y ) {
		Matrix33* active = getActiveMatrix();
		Matrix33 translate;
		translate.setTranslate( x, y );
		active->multiply( translate );
	}
	void OpenGLContext::rotate( VGfloat angle ) {
		Matrix33* active = getActiveMatrix();
		Matrix33 rotate;
		rotate.setRotate( radians( angle ) );
		active->multiply( rotate );
	}
	
	
	//// singleton implementation ////
	static OpenGLContext g_context;
	IContext& IContext::instance()
	{
		return g_context;
	}
	
}