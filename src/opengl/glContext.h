/*
 *  glContext.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glContext_h__
#define __glContext_h__

#include "mkContext.h"
#include "glPlatform.h"
#include "OpenGLES11Context.h"
#include "OpenGLES20Context.h"
#undef CHECK_GL_ERROR	// gles2-bc also leaves this defined.

namespace MonkVG {
    
#define GL (((OpenGLContext*)&IContext::instance())->gl())
	
	// todo: setup debug and release versions
	//#define CHECK_GL_ERROR OpenGLContext::checkGLError()
	#define CHECK_GL_ERROR
	class OpenGLContext : public IContext {
	public:
	
		OpenGLContext();
		
		virtual bool Initialize();
		virtual bool Terminate();
		
		//// factories ////
		virtual IPath* createPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities );
		virtual void destroyPath( IPath* path );	
		virtual void destroyPaint( IPaint* paint );
		virtual IPaint* createPaint();
		virtual IImage* createImage( VGImageFormat format,
									VGint width, VGint height,
									VGbitfield allowedQuality );
		virtual void destroyImage( IImage* image );
		virtual IBatch* createBatch();
		virtual void destroyBatch( IBatch* batch );
		virtual IFont* createFont();
		virtual void destroyFont( IFont* font );


		/// paint overrides
		virtual void setStrokePaint( IPaint* paint );
		virtual void setFillPaint( IPaint* paint );

		//// platform specific execution of stroke and fill ////
		virtual void stroke();
		virtual void fill();
		
		//// platform specific execution of Masking and Clearing ////
		virtual void clear(VGint x, VGint y, VGint width, VGint height);
		
		//// platform specific implementation of transform ////
		virtual void setIdentity();
		virtual void transform( VGfloat* t );
		virtual void scale( VGfloat sx, VGfloat sy );
		virtual void translate( VGfloat x, VGfloat y );
		virtual void rotate( VGfloat angle );
		virtual void setTransform( const VGfloat* t ) ;
		virtual void multiply( const VGfloat* t );
		virtual void setMatrixMode( VGMatrixMode mode ) {
			IContext::setMatrixMode( mode );
			loadGLMatrix();
		}
		void loadGLMatrix();

		
		
		
		void beginRender();
		void endRender();
		
		
		virtual void resize();
		
		
		static void checkGLError();
		
		/// batch drawing
		virtual void startBatch( IBatch* batch );
        virtual void dumpBatch( IBatch* batch, void **vertices, size_t *size );
		virtual void endBatch( IBatch* batch );
		
		/// image
		virtual void setImageMode( VGImageMode im );
        
        
        OpenGLES::OpenGLESContext* getGLESBackendContext() { return _gl; }
        OpenGLES::OpenGLESContext* gl() { return getGLESBackendContext(); }

	private:
		
		// restore values to play nice with other apps
		int		_viewport[4];
		float	_projection[16];
		float	_modelview[16];
		float	_color[4];
        
        // the gl context
        OpenGLES::OpenGLESContext*  _gl;
	};
}

#endif // __qlContext_h__
