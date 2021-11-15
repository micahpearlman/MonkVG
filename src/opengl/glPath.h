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
#include "glPlatform.h"
#include <list>
#include <vector>
#include "glPaint.h"

namespace MonkVG {
	class glBatch;
	
	class OpenGLPath : public IPath {
	public:
	
		OpenGLPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) 
			:	IPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities )
			,	_fillPaintForPath( 0 )
			,	_strokePaintForPath( 0 )
		{

		}
		virtual ~OpenGLPath();
		
		
		virtual bool draw( VGbitfield paintModes );
		virtual void clear( VGbitfield caps );
		virtual void buildFillIfDirty();
		void buildStrokeIfDirty();

		void appendStrokeData(VGint nseg, const VGubyte * segments, const void * data);


	private:
		struct v2_t {
			GLfloat x, y;
		};
		
		struct v3_t {
			GLdouble x,y,z;
			v3_t() {}
	       		v3_t( GLdouble * v) : x(v[0]), y(v[1]), z(v[2]) {}
			v3_t(GLdouble ix, GLdouble iy, GLdouble iz) : x(ix), y(iy), z(iz) {}
			void print() const {
				printf("(%f, %f)\n", x, y);
			}
			
		};
		
		struct textured_vertex_t {
			GLfloat		v[2];
			GLfloat		uv[2];
		};
		
	private:
		vector<GLfloat>		_fillVertices;
		vector<GLfloat>		_strokeVertices;
		vector<GLfloat>		*_vertices;
		list<v3_t>		_fillTessVertices;
		list<v3_t>		_strokeTessVertices;
		list<v3_t>		*_tessVertices;


		vector<VGubyte> _strokeSegments;
		vector<GLfloat> _strokeData;

		GLenum				_primType;
		GLvoid*				_fillOffset;
		GLvoid*				_strokeOffset;
		int					_numberFillVertices;
		int					_numberStrokeVertices;
		OpenGLPaint*		_fillPaintForPath;
		OpenGLPaint*		_strokePaintForPath;
		
		
	private:		// tesseleator callbacks
		static void tessBegin( GLenum type, GLvoid* user );
		static void tessEnd( GLvoid* user );
		static void tessVertex( GLvoid *vertex, GLvoid* user );
		static void tessCombine( GLdouble coords[3], void *data[4],
								GLfloat weight[4], void **outData,
								void *polygonData );
		static void tessError( GLenum errorCode );
		static void tessEdgeFlag( GLboolean f, void * data ) {}
		void endOfTesselation( VGbitfield paintModes );
		
	private:	// utility methods
		
		GLenum primType() {
			return _primType;
		}
		void setPrimType( GLenum t ) {
			_primType = t;
		}
		
		GLdouble* tessVerticesBackPtr() {
			return &(_tessVertices->back().x);
		} 
		
		void updateBounds(float x, float y) {
			_minX = std::min(_minX, x);
			_width = std::max(_width, x);
			_minY = std::min(_minY, y);
			_height = std::max(_height, y);
		}

		void addVertex( GLdouble* v ) {
			VGfloat x = (VGfloat)v[0];
			VGfloat y = (VGfloat)v[1];
			updateBounds(x, y);
			_vertices->push_back(x);
			_vertices->push_back(y);
		}

		GLdouble * addTessVertex( const v3_t & v ) {
			_tessVertices->push_back( v );
			return tessVerticesBackPtr();
		}
		
		void buildFill();
		void buildStroke();
		void buildGen(vector<VGubyte> &segments, vector<VGfloat> &coords);

		bool usesTexture() const;

	};
}

#endif // __glPath_h__
