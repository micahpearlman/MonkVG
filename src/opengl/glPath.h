/*
 *  glPath.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glPath_h__
#define __glPath_h__

#include "mkPath.h"
#if defined(__APPLE__)
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#include "glu.h"
	#else
		#include <OpenGL/glu.h>
		#define glOrthof glOrtho
	#endif
#endif // #if defined(__APPLE__)


#include <list>
#include <vector>

namespace MonkVG {
	
	class OpenGLPath : public IPath {
	public:
	
		OpenGLPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) 
			:	IPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities )
			,	_fillTesseleator( 0 )
			,	_strokeVBO(-1)
			,	_fillVBO(-1)
		{

		}
		virtual ~OpenGLPath();
		
		
		virtual bool draw( VGbitfield paintModes );
		virtual void clear( VGbitfield caps );
		
		
	public:		// really private but accessors for tesselator
		GLenum primType() {
			return _primType;
		}
		void setPrimType( GLenum t ) {
			_primType = t;
		}
		void addVertex( GLdouble* v ) {
			_vertices.push_back( v[0] );
			_vertices.push_back( v[1] );
		}
		void addVertexToDestroy( GLdouble* v ) {
			_verticesToDestroy.push_back( v );
		}
		
	private:
		
		GLUtesselator*		_fillTesseleator;
		vector<GLfloat>		_vertices;
		list<GLdouble*>		_verticesToDestroy;
		GLenum				_primType;
		GLuint				_fillVBO;
		GLuint				_strokeVBO;
		int					_numberFillVertices;
		int					_numberStrokeVertices;
		
	private:		// tesseleator callbacks
		static void tessBegin( GLenum type, GLvoid* user );
		static void tessEnd( GLvoid* user );
		static void tessVertex( GLvoid *vertex, GLvoid* user );
		static void tessCombine( GLdouble coords[3], void *data[4],
								GLfloat weight[4], void **outData,
								void *polygonData );
		static void tessError( GLenum errorCode );
		void endOfTesselation();
		
	private:	// utility methods
		

		void buildFill();
		void buildStroke();
		
		
	};
}

#endif // __glPath_h__