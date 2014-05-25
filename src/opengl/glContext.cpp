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
#include "glBatch.h"
#include "glImage.h"
#include "glFont.h"
#include "mkCommon.h"


namespace MonkVG {
	
	//// singleton implementation ////
	IContext& IContext::instance()
	{
        static OpenGLContext g_context;
		return g_context;
	}

	
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
	
		CHECK_GL_ERROR;
        
        // create the gl backend context dependent on user selected backend
        if ( getRenderingBackendType() == VG_RENDERING_BACKEND_TYPE_OPENGLES11 ) {
            _gl = new OpenGLES::OpenGLES1::OpenGLES11Context();
        } else if ( getRenderingBackendType() == VG_RENDERING_BACKEND_TYPE_OPENGLES20 ) {
            _gl = new OpenGLES::OpenGLES2::OpenGLES20Context();
        } else {    // error
            MK_ASSERT( !"ERROR: No OpenGL rendering backend selected" );
        }
		
		// get viewport to restore back when we are done
		gl()->glGetIntegerv( GL_VIEWPORT, _viewport );
//fixme?		gl()->glGetFloatv( GL_PROJECTION_MATRIX, _projection );
//fixme?		gl()->glGetFloatv( GL_MODELVIEW_MATRIX, _modelview );
		
		// get the color to back up when we are done
		gl()->glGetFloatv( GL_CURRENT_COLOR, _color );
		
		resize();
		
		gl()->glDisable(GL_CULL_FACE);
		gl()->glDisable(GL_TEXTURE_2D);
		
		// turn on blending
		gl()->glEnable(GL_BLEND);
		gl()->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		gl()->glDisable(GL_TEXTURE_2D);
		gl()->glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		gl()->glDisableClientState( GL_COLOR_ARRAY );
		gl()->glEnableClientState( GL_VERTEX_ARRAY );
		
		
		CHECK_GL_ERROR;
		
		return true;
	}
	
	void OpenGLContext::resize() {
		// setup GL projection 
		gl()->glViewport(0,0, _width, _height);
		
		gl()->glMatrixMode(GL_PROJECTION);
		gl()->glLoadIdentity();
		gl()->glOrthof(0, _width,		// left, right
				 0, _height,	// top, botton
				 -1, 1);		// near value, far value (depth)
		
		gl()->glMatrixMode(GL_MODELVIEW);
		gl()->glLoadIdentity();
	}
	
	
	bool OpenGLContext::Terminate() {
        if (_gl) {
            delete _gl;
            _gl = NULL;
        }
        _stroke_paint = NULL;
        _fill_paint = NULL;
		return true;
	}
	
	
	void OpenGLContext::beginRender() {
//		glDisable(GL_TEXTURE_2D);
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//		glDisableClientState( GL_COLOR_ARRAY );
//		glEnableClientState( GL_VERTEX_ARRAY );
		
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);			

		
//		CHECK_GL_ERROR;
//		
//		// get viewport to restore back when we are done
//		glGetIntegerv( GL_VIEWPORT, _viewport );
//		glGetFloatv( GL_PROJECTION_MATRIX, _projection );
//		glGetFloatv( GL_MODELVIEW_MATRIX, _modelview );
//		
//		// get the color to back up when we are done
//		glGetFloatv( GL_CURRENT_COLOR, _color );
//		
//		// setup GL projection 
//		glViewport(0,0, _width, _height);
//		
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		glOrthof(0, _width,		// left, right
//				 0, _height,	// top, botton
//				 -1, 1);		// near value, far value (depth)
//		
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		
//		glDisable( GL_CULL_FACE );
//		
//		// turn on blending
//		glEnable(GL_BLEND);
//		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
//		
//		CHECK_GL_ERROR;
		
	}
	void OpenGLContext::endRender() {
//		
//		CHECK_GL_ERROR;
//		
//		// todo: restore state to be nice to other apps
//		// restore the old viewport
//		glMatrixMode( GL_PROJECTION );
//		glLoadMatrixf( _projection );
//		glViewport( _viewport[0], _viewport[1], _viewport[2], _viewport[3] );
//		glMatrixMode( GL_MODELVIEW );			
//		glLoadMatrixf( _modelview );
//		
//		// restore color
//		glColor4f( _color[0], _color[1], _color[2], _color[3] );
//		
//		CHECK_GL_ERROR;
	}

	
	/// factories
	
	IPath* OpenGLContext::createPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) {
	
		OpenGLPath *path = new OpenGLPath(pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities  &= VG_PATH_CAPABILITY_ALL);
		if( path == 0 )
			SetError( VG_OUT_OF_MEMORY_ERROR );
	
		return (IPath*)path;
	}
	
	void OpenGLContext::destroyPath( IPath* path ) {
		delete (OpenGLPath*)path;
	}
	
	void OpenGLContext::destroyPaint( IPaint* paint ) {
		delete (OpenGLPaint*)paint;
	}
	
	IPaint* OpenGLContext::createPaint() {
		OpenGLPaint *paint = new OpenGLPaint();
		if( paint == 0 )
			SetError( VG_OUT_OF_MEMORY_ERROR );
		return (IPaint*)paint;
	}
	
	IBatch* OpenGLContext::createBatch() {
		OpenGLBatch *batch = new OpenGLBatch();
		if( batch == 0 )
			SetError( VG_OUT_OF_MEMORY_ERROR );
		return (IBatch*)batch;
	}
	
	void OpenGLContext::destroyBatch( IBatch* batch ) {
		if ( batch ) {
			delete batch;
		}
	}
	
	IImage* OpenGLContext::createImage( VGImageFormat format,
									   VGint width, VGint height,
									   VGbitfield allowedQuality) {
		return new OpenGLImage( format, width, height, allowedQuality );
	}
	void OpenGLContext::destroyImage( IImage* image ) {
		if ( image ) {
			delete image;
		}
	}
	
	IFont* OpenGLContext::createFont() {
		return new OpenGLFont();
	}
	void OpenGLContext::destroyFont( IFont* font ) {
		if ( font ) {
			delete font;
		}
	}



	/// state 
	void OpenGLContext::setStrokePaint( IPaint* paint ) {
		if ( paint != _stroke_paint ) {
			IContext::setStrokePaint( paint );
			OpenGLPaint* glPaint = (OpenGLPaint*)_stroke_paint;
			//glPaint->setGLState();
                        if (glPaint)
                            glPaint->setIsDirty( true );
		}
	}
	
	void OpenGLContext::setFillPaint( IPaint* paint ) {
		if ( paint != _fill_paint ) {
			IContext::setFillPaint( paint );
			OpenGLPaint* glPaint = (OpenGLPaint*)_fill_paint;
			//glPaint->setGLState();
                        if (glPaint)
                            glPaint->setIsDirty( true );
		}
		
	}

	
	void OpenGLContext::stroke() {
		if ( _stroke_paint ) {
			OpenGLPaint* glPaint = (OpenGLPaint*)_stroke_paint;
			glPaint->setGLState();
			glPaint->setIsDirty( false );
			// set the fill paint to dirty
			if ( _fill_paint ) {
				glPaint = (OpenGLPaint*)_fill_paint;
				glPaint->setIsDirty( true );
			}
		}
	}
	
	void OpenGLContext::fill() {
		
		if ( _fill_paint && _fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR ) {
			OpenGLPaint* glPaint = (OpenGLPaint*)_fill_paint;
			glPaint->setGLState();
			glPaint->setIsDirty( false );
			// set the stroke paint to dirty
			if ( _stroke_paint ) {
				glPaint = (OpenGLPaint*)_stroke_paint;
				glPaint->setIsDirty( true );
			}
		}

//		if ( _fill_paint ) {
//			const VGfloat* c = _fill_paint->getPaintColor();
//			glColor4f(c[0], c[1], c[2], c[3] );
//		}
		
	}
	
	void OpenGLContext::startBatch( IBatch* batch ) {
		assert( _currentBatch == 0 );	// can't have multiple batches going on at once
		_currentBatch = batch;
	}
    void OpenGLContext::dumpBatch( IBatch *batch, void **vertices, size_t *size ) {
        _currentBatch->dump( vertices, size );
    }
	void OpenGLContext::endBatch( IBatch* batch ) {
		_currentBatch->finalize();
		_currentBatch = 0;
	}

	
	void OpenGLContext::clear(VGint x, VGint y, VGint width, VGint height) {
		// TODO:
	}
	
	void OpenGLContext::loadGLMatrix() {
		Matrix33& active = *getActiveMatrix();
		GLfloat mat44[4][4];
		for( int x = 0; x < 4; x++ )
			for( int y = 0; y < 4; y++ )
				mat44[x][y] = 0;
		mat44[0][0] = 1.0f;
		mat44[1][1] = 1.0f;
		mat44[2][2] = 1.0f;
		mat44[3][3]	= 1.0f;
		
		
		//		a, c, e,			// cos(a) -sin(a) tx
		//		b, d, f,			// sin(a) cos(a)  ty
		//		ff0, ff1, ff2;		// 0      0       1
		
		mat44[0][0] = active.a;	mat44[0][1] = active.b;
		mat44[1][0] = active.c;	mat44[1][1] = active.d;
		mat44[3][0] = active.e;	mat44[3][1] = active.f;
		gl()->glLoadMatrixf( &mat44[0][0] );
		
	}
	
	
	
	void OpenGLContext::setIdentity() {
		Matrix33* active = getActiveMatrix();
		active->setIdentity();
		loadGLMatrix();
	}
	
	void OpenGLContext::transform( VGfloat* t ) {
		// a	b	0
		// c	d	0
		// tx	ty	1
		Matrix33* active = getActiveMatrix();
		for( int i = 0; i < 9; i++ )
			t[i] = active->m[i];
		
	}
	
	void OpenGLContext::setTransform( const VGfloat* t )  {
		//	OpenVG:
		//	sh	shx	tx
		//	shy	sy	ty
		//	0	0	1
		//
		// OpenGL
		// a	b	0
		// c	d	0
		// tx	ty	1
		
		Matrix33* active = getActiveMatrix();
		for( int i = 0; i < 9; i++ )
			active->m[i] = t[i];
		loadGLMatrix();
	}
	
	
	void OpenGLContext::multiply( const VGfloat* t ) {
		Matrix33 m;
		for ( int x = 0; x < 3; x++ ) {
			for ( int y = 0; y < 3; y++ ) {
				m.set( y, x, t[(y*3)+x] );
			}
		}
		Matrix33* active = getActiveMatrix();
		active->postMultiply( m );
		loadGLMatrix();
	}
	
	void OpenGLContext::scale( VGfloat sx, VGfloat sy ) {
		Matrix33* active = getActiveMatrix();
		Matrix33 scale;
		scale.setIdentity();
		scale.setScale( sx, sy );
        Matrix33 tmp;
        Matrix33::multiply( tmp, scale, *active );
		active->copy( tmp );
		loadGLMatrix();
	}
	void OpenGLContext::translate( VGfloat x, VGfloat y ) {
		
		Matrix33* active = getActiveMatrix();
		Matrix33 translate;
		translate.setTranslate( x, y );
		Matrix33 tmp;
		tmp.setIdentity();
		Matrix33::multiply( tmp, translate, *active );
		active->copy( tmp );
		loadGLMatrix();
	}
	void OpenGLContext::rotate( VGfloat angle ) {
		Matrix33* active = getActiveMatrix();
		Matrix33 rotate;
		rotate.setRotation( radians( angle ) );
		Matrix33 tmp;
		tmp.setIdentity();
		Matrix33::multiply( tmp, rotate, *active );
		active->copy( tmp );
		loadGLMatrix();
	}
	
	void OpenGLContext::setImageMode( VGImageMode im ) {
		IContext::setImageMode( im );
		switch ( im ) {
			case VG_DRAW_IMAGE_NORMAL:
				gl()->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				break;
			case VG_DRAW_IMAGE_MULTIPLY:
				gl()->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				break;
			case VG_DRAW_IMAGE_STENCIL:
				break;
			default:
				break;
		}
	}
	
	
	
}
