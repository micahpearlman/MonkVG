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
			
			// calculate the bounds
			if ( v[0] < _minX ) {
				_minX = v[0];
			}
			if ( v[0] > _width ) {
				_width = v[0];
			}
			if ( v[1] < _minY ) {
				_minY = v[0];
			}
			if ( v[1] > _height ) {
				_width = v[0];
			}
			
			_vertices.push_back( v[0] );
			_vertices.push_back( v[1] );
		}
		
		GLdouble* tessVerticesBackPtr() {
			return &(_tessVertices.back().x);
		} 
		
		struct v2_t {
			GLfloat x, y;
			
			void print() const {
				printf("(%f, %f)\n", x, y);
			}
		};
		
		
		struct v3_t {
			GLdouble x,y,z;
			v3_t( const v2_t& v ) : x(v.x), y(v.y), z(0) {}
			v3_t() : x(0), y(0), z(0) {}
			void print() const {
				printf("(%f, %f)\n", x, y);
			}
			
		};
		
		
		void addTessVertex( const v3_t& v ) {
			_tessVertices.push_back( v );
		}
		
		
	private:
		
		
		
		
		GLUtesselator*		_fillTesseleator;
		vector<GLfloat>		_vertices;
		list<v3_t>		_tessVertices;
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
		void buildFatLineSegment( vector<v2_t>& vertices, const v2_t& p0, const v2_t& p1, const float radius );
		
		
	};
}

#endif // __glPath_h__