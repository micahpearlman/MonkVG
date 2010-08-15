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

namespace MonkVG {
	
	OpenGLContext::OpenGLContext()
		:	IContext()
	{
	}
	
	
	bool OpenGLContext::Initialize() {
	
		/* setup GL projection */
		glViewport(0,0, _width, _height);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(0, _width, _height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		return true;
	}
	
	
	bool OpenGLContext::Terminate() {
		return true;
	}
	
	void OpenGLContext::beginRender() {
		/* setup GL projection */
		glViewport(0,0, _width, _height);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(0, _width, _height, 0, -1, 1);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glDisable( GL_CULL_FACE );
	}
	void OpenGLContext::endRender() {
		//glEnable( GL_CULL_FACE );
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