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
#include "glu.h"
#include <list>
#include <vector>

namespace MonkVG {
	
	class OpenGLPath : public IPath {
	public:
	
		OpenGLPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) 
			:	IPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities )
			,	_fillTesseleator( 0 )
			,	_isDirty( true )
		{

		}
		virtual ~OpenGLPath();
		
		
		virtual bool draw( VGbitfield paintModes );
		
		
	public:		// really private but accessors for tesselator
		GLenum primType() {
			return _primType;
		}
		void setPrimType( GLenum t ) {
			_primType = t;
		}
		void addVertex( GLfloat* v ) {
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
		int					_numberVertices;
		bool				_isDirty;
		
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
		static inline VGfloat calcCubicBezier1d( VGfloat x0, VGfloat x1, VGfloat x2, VGfloat x3, VGfloat t ) {
			// see openvg 1.0 spec section 8.3.2 Cubic Bezier Curves
			VGfloat oneT = 1.0f - t;
			VGfloat x =		x0 * (oneT * oneT * oneT)
						+	3.0f * x1 * (oneT * oneT) * t
						+	3.0f * x2 * oneT * (t * t)
						+	x3 * (t * t * t);
			return x;	
		}
		
	};
}

#endif // __glPath_h__