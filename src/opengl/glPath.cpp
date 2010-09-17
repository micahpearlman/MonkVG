/*
 *  glPath.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */


#include "glPath.h"
#include "glContext.h"
#include <cassert>

namespace MonkVG {
	
	bool OpenGLPath::draw( VGbitfield paintModes ) {
	
		// get the native OpenGL context
		OpenGLContext& glContext = (MonkVG::OpenGLContext&)IContext::instance();

		if( paintModes & VG_FILL_PATH && _isDirty == true ) {	// build the fill polygons
			buildFill();
		}
		
		if( paintModes & VG_STROKE_PATH && _isDirty == true ) {
			buildStroke();
		}
		
		_isDirty = false;
		
		glContext.beginRender();
		
		
		
		Matrix33 active = *IContext::instance().getActiveMatrix();
		// a	b	0
		// c	d	0
		// tx	ty	1
		//active.transpose();		// NOTE:  have to transpose.  Maybe should set up the matrices already as transposed?
		
		GLfloat mat44[4][4];
		for( int x = 0; x < 4; x++ )
			for( int y = 0; y < 4; y++ )
				mat44[x][y] = 0;
		mat44[0][0] = active.get( 0, 0 );
		mat44[0][1] = active.get( 0, 1 );
		mat44[1][0]	= active.get( 1, 0 );
		mat44[1][1] = active.get( 1, 1 );
		mat44[2][2] = 1.0f;
		mat44[3][3]	= 1.0f;
		mat44[3][0] = active.get( 0, 2 );
		mat44[3][1] = active.get( 1, 2 );
		//todo: translation
		glPushMatrix();
		glLoadMatrixf( &mat44[0][0] );
		if( paintModes & VG_FILL_PATH ) {
			
			// draw
			IContext::instance().fill();
			glBindBuffer( GL_ARRAY_BUFFER, _fillVBO );
			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer( 2, GL_FLOAT, sizeof(float) * 2, 0 );
			glDrawArrays( GL_TRIANGLES, 0, _numberFillVertices );

		}
		
		if ( paintModes & VG_STROKE_PATH ) {
			IContext::instance().stroke();
			glBindBuffer( GL_ARRAY_BUFFER, _strokeVBO );
			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer( 2, GL_FLOAT, sizeof(float) * 2, 0 );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, _numberStrokeVertices );
			
		}
		
		glContext.endRender();
		glPopMatrix();
		
		return true;
	}
	
	static inline VGfloat calcCubicBezier1d( VGfloat x0, VGfloat x1, VGfloat x2, VGfloat x3, VGfloat t ) {
		// see openvg 1.0 spec section 8.3.2 Cubic Bezier Curves
		VGfloat oneT = 1.0f - t;
		VGfloat x =		x0 * (oneT * oneT * oneT)
		+	3.0f * x1 * (oneT * oneT) * t
		+	3.0f * x2 * oneT * (t * t)
		+	x3 * (t * t * t);
		return x;	
	}
	
	void OpenGLPath::buildFill() {
		
		_fillTesseleator = gluNewTess();
		gluTessCallback( _fillTesseleator, GLU_TESS_BEGIN_DATA, (GLvoid (*) ( )) &OpenGLPath::tessBegin );
		gluTessCallback( _fillTesseleator, GLU_TESS_END_DATA, (GLvoid (*) ( )) &OpenGLPath::tessEnd );
		gluTessCallback( _fillTesseleator, GLU_TESS_VERTEX_DATA, (GLvoid (*) ( )) &OpenGLPath::tessVertex );
		gluTessCallback( _fillTesseleator, GLU_TESS_COMBINE_DATA, (GLvoid (*) ( )) &OpenGLPath::tessCombine );
		gluTessCallback( _fillTesseleator, GLU_TESS_ERROR, (GLvoid (*)())&OpenGLPath::tessError );
		// TODO: SET WINDING RULE BASED ON THE OPENVG SETTING!!!
		gluTessProperty( _fillTesseleator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD ); 
		
		gluTessBeginPolygon( _fillTesseleator, this );
		
		
		vector< VGfloat >::iterator coordsIter = _fcoords->begin();
		int numCoords = 0;
		VGbyte segment = VG_CLOSE_PATH;
		GLdouble coords[3] = {0,0,0};
		GLdouble closeTo[3] = {0,0,0};
		int num_contours = 0;
		for ( vector< VGubyte >::iterator segmentIter = _segments.begin(); segmentIter != _segments.end(); segmentIter++ ) {
			segment = (*segmentIter);
			numCoords = segmentToNumCoordinates( static_cast<VGPathSegment>( segment ) );
			//segment = segment >> 1;
			
			
			//			VG_CLOSE_PATH                               = ( 0 << 1),
			//			VG_MOVE_TO                                  = ( 1 << 1),
			//			VG_LINE_TO                                  = ( 2 << 1),
			//			VG_HLINE_TO                                 = ( 3 << 1),
			//			VG_VLINE_TO                                 = ( 4 << 1),
			//			VG_QUAD_TO                                  = ( 5 << 1),
			//			VG_CUBIC_TO                                 = ( 6 << 1),
			//			VG_SQUAD_TO                                 = ( 7 << 1),
			//			VG_SCUBIC_TO                                = ( 8 << 1),
			//			VG_SCCWARC_TO                               = ( 9 << 1),
			//			VG_SCWARC_TO                                = (10 << 1),
			//			VG_LCCWARC_TO                               = (11 << 1),
			//			VG_LCWARC_TO                                = (12 << 1),
			
			// todo: deal with relative move
			switch (segment >> 1) {
				case (VG_CLOSE_PATH >> 1):
				{
					GLdouble* c = new GLdouble[3];
					c[0] = closeTo[0];
					c[1] = closeTo[1];
					c[2] = closeTo[2];
					// do not think this is necessary for the tesselator						
					//gluTessVertex( _fillTesseleator, c, c );
				} break;
				case (VG_MOVE_TO >> 1):
				{	
					if ( num_contours ) {
						gluTessEndContour( _fillTesseleator );
						num_contours--;
					}
					
					gluTessBeginContour( _fillTesseleator );
					num_contours++;
					closeTo[0] = coords[0] = *coordsIter; coordsIter++;
					closeTo[1] = coords[1] = *coordsIter; coordsIter++;
					
					GLdouble* c = new GLdouble[3];
					c[0] = coords[0];
					c[1] = coords[1];
					c[2] = coords[2];
					
					gluTessVertex( _fillTesseleator, c, c );
					
				} break;
				case (VG_LINE_TO >> 1):
				{
					coords[0] = *coordsIter; coordsIter++;
					coords[1] = *coordsIter; coordsIter++;
					
					GLdouble* c = new GLdouble[3];
					c[0] = coords[0];
					c[1] = coords[1];
					c[2] = coords[2];
					
					gluTessVertex( _fillTesseleator, c, c );
				} break;
				case (VG_HLINE_TO >> 1):
				{
					coords[0] = *coordsIter; coordsIter++;
					
					GLdouble* c = new GLdouble[3];
					c[0] = coords[0];
					c[1] = coords[1];
					c[2] = coords[2];
					
					gluTessVertex( _fillTesseleator, c, c );
				} break;
				case (VG_VLINE_TO >> 1):
				{
					coords[1] = *coordsIter; coordsIter++;
					
					GLdouble* c = new GLdouble[3];
					c[0] = coords[0];
					c[1] = coords[1];
					c[2] = coords[2];
					
					gluTessVertex( _fillTesseleator, c, c );
				} break;
				case (VG_CUBIC_TO >> 1):	// todo
				{
					VGfloat cp1x = *coordsIter; coordsIter++;
					VGfloat cp1y = *coordsIter; coordsIter++;
					VGfloat cp2x = *coordsIter; coordsIter++;
					VGfloat cp2y = *coordsIter; coordsIter++;
					VGfloat p3x = *coordsIter; coordsIter++;
					VGfloat p3y = *coordsIter; coordsIter++;
					
					VGfloat increment = 1.0f / 4.0f;
					//printf("\tcubic: ");
					for ( VGfloat t = increment; t < 1.0f + increment; t+=increment ) {
						GLdouble* c = new GLdouble[3];
						c[0] = calcCubicBezier1d( coords[0], cp1x, cp2x, p3x, t );
						c[1] = calcCubicBezier1d( coords[1], cp1y, cp2y, p3y, t );
						c[2] = coords[2];
						//printf( "(%f, %f), ", c[0], c[1] );
						gluTessVertex( _fillTesseleator, c, c );
					}
					//printf("\n");
					coords[0] = p3x;
					coords[1] = p3y;
					
				}
					break;	
				default:
					printf("unkwown command\n");
					break;
			}
		}	// foreach segment
		
		if ( num_contours ) {
			gluTessEndContour( _fillTesseleator );
			num_contours--;
		}

		assert(num_contours == 0);
		
		gluTessEndPolygon( _fillTesseleator );
		
		gluDeleteTess( _fillTesseleator );
		
		
		endOfTesselation();
		
		_fillTesseleator = 0;
		
	}

	struct v2_t {
		GLfloat x, y;
		
		void print() const {
			//printf("(%f, %f)\n", x, y);
		}
	};
	static inline void buildFatLineSegment( vector<v2_t>& vertices, const v2_t& p0, const v2_t& p1, const float radius ) {
		
		if ( (p0.x == p1.x) && (p0.y == p1.y ) ) {
			return;
		}
			
		float dx = p1.y - p0.y;
		float dy = p0.x - p1.x;
		const float inv_mag = 1.0f / sqrtf(dx*dx + dy*dy);
		dx = dx * inv_mag;
		dy = dy * inv_mag;

		v2_t v0, v1, v2, v3;
		
		v0.x = p0.x + radius * dx;
		v0.y = p0.y + radius * dy;
		vertices.push_back( v0 );
		
		v1.x = p0.x - radius * dx;
		v1.y = p0.y - radius * dy;
		vertices.push_back( v1 );			
		
		
		v2.x = p1.x + radius * dx;
		v2.y = p1.y + radius * dy;
		vertices.push_back( v2 );			
		
		v3.x = p1.x - radius * dx;
		v3.y = p1.y - radius * dy;
		vertices.push_back( v3 );
		
//		printf("start stroke\n");
//		printf("p0: ");p0.print();
//		printf("p1: ");p1.print();
//		printf("\t"); v0.print();
//		printf("\t"); v1.print();
//		printf("\t"); v2.print();
//		printf("\t"); v3.print();
//		printf("end stroke\n");
		
	}
	
	void OpenGLPath::buildStroke() {
		
		// get the native OpenGL context
		OpenGLContext& glContext = (MonkVG::OpenGLContext&)IContext::instance();
		
		const VGfloat stroke_width = glContext.getStrokeLineWidth();
		
		vector< VGfloat >::iterator coordsIter = _fcoords->begin();
		int numCoords = 0;
		VGbyte segment = VG_CLOSE_PATH;
		v2_t coords = {0,0};
		v2_t prev = {0,0};
		v2_t closeTo = {0,0}; 
		vector<v2_t> vertices;
		for ( vector< VGubyte >::iterator segmentIter = _segments.begin(); segmentIter != _segments.end(); segmentIter++ ) {
			segment = (*segmentIter);
			numCoords = segmentToNumCoordinates( static_cast<VGPathSegment>( segment ) );
			//segment = segment >> 1;
			
			
			//			VG_CLOSE_PATH                               = ( 0 << 1),
			//			VG_MOVE_TO                                  = ( 1 << 1),
			//			VG_LINE_TO                                  = ( 2 << 1),
			//			VG_HLINE_TO                                 = ( 3 << 1),
			//			VG_VLINE_TO                                 = ( 4 << 1),
			//			VG_QUAD_TO                                  = ( 5 << 1),
			//			VG_CUBIC_TO                                 = ( 6 << 1),
			//			VG_SQUAD_TO                                 = ( 7 << 1),
			//			VG_SCUBIC_TO                                = ( 8 << 1),
			//			VG_SCCWARC_TO                               = ( 9 << 1),
			//			VG_SCWARC_TO                                = (10 << 1),
			//			VG_LCCWARC_TO                               = (11 << 1),
			//			VG_LCWARC_TO                                = (12 << 1),
			
			// todo: deal with relative move
			switch (segment >> 1) {
				case (VG_CLOSE_PATH >> 1):
				{
					buildFatLineSegment( vertices, coords, closeTo, stroke_width );
				} break;
				case (VG_MOVE_TO >> 1):
				{	
					prev.x = closeTo.x = coords.x = *coordsIter; coordsIter++;
					prev.y = closeTo.y = coords.y = *coordsIter; coordsIter++;
					
				} break;
				case (VG_LINE_TO >> 1):
				{
					prev = coords;
					coords.x = *coordsIter; coordsIter++;
					coords.y = *coordsIter; coordsIter++;
					
					buildFatLineSegment( vertices, prev, coords, stroke_width );

					
				} break;
				case (VG_HLINE_TO >> 1):
				{
					prev = coords;
					coords.x = *coordsIter; coordsIter++;
					buildFatLineSegment( vertices, prev, coords, stroke_width );
				} break;
				case (VG_VLINE_TO >> 1):
				{
					prev = coords;
					coords.y = *coordsIter; coordsIter++;
					buildFatLineSegment( vertices, prev, coords, stroke_width );
					
				} break;
				case (VG_CUBIC_TO >> 1):	// todo
				{
					VGfloat cp1x = *coordsIter; coordsIter++;
					VGfloat cp1y = *coordsIter; coordsIter++;
					VGfloat cp2x = *coordsIter; coordsIter++;
					VGfloat cp2y = *coordsIter; coordsIter++;
					VGfloat p3x = *coordsIter; coordsIter++;
					VGfloat p3y = *coordsIter; coordsIter++;
					
					VGfloat increment = 1.0f / 4.0f;
					prev = coords;
					for ( VGfloat t = increment; t < 1.0f + increment; t+=increment ) {
						v2_t c;
						c.x = calcCubicBezier1d( coords.x, cp1x, cp2x, p3x, t );
						c.y = calcCubicBezier1d( coords.y, cp1y, cp2y, p3y, t );
						buildFatLineSegment( vertices, prev, c, stroke_width );
						prev = c;
					}
					coords.x = p3x;
					coords.y = p3y;
					
				}
					break;	
				default:
					printf("unkwown command\n");
					break;
			}
		}	// foreach segment
		
		// build the vertex buffer object VBO
		glGenBuffers( 1, &_strokeVBO );
		glBindBuffer( GL_ARRAY_BUFFER, _strokeVBO );
		glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 2, &vertices[0], GL_STATIC_DRAW );
		_numberStrokeVertices = vertices.size();
		
		
	}
	
	void OpenGLPath::endOfTesselation() {
		glGenBuffers( 1, &_fillVBO );
		glBindBuffer( GL_ARRAY_BUFFER, _fillVBO );
		glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(float) * 2, &_vertices[0], GL_STATIC_DRAW );
		_numberFillVertices = _vertices.size()/2;
		for (list<GLdouble*>::iterator iter = _verticesToDestroy.begin(); iter != _verticesToDestroy.end(); iter++ ) {
//todo!!!			delete [] *(iter);
		}
		_verticesToDestroy.clear();
		_vertices.clear();
	}

	static GLfloat startVertex_[2];
	static GLfloat lastVertex_[2];
	static int vertexCount_ = 0;
	
	void OpenGLPath::tessBegin( GLenum type, GLvoid* user ) {
		OpenGLPath* me = (OpenGLPath*)user;
		me->setPrimType( type );
		vertexCount_ = 0;
		
		switch( type )
		{
			case GL_TRIANGLES:
				//printf( "begin(GL_TRIANGLES)\n" );
				break;
			case GL_TRIANGLE_FAN:
				//printf( "begin(GL_TRIANGLE_FAN)\n" );
				break;
			case GL_TRIANGLE_STRIP:
				//printf( "begin(GL_TRIANGLE_STRIP)\n" );
				break;
			case GL_LINE_LOOP:
				//printf( "begin(GL_LINE_LOOP)\n" );
				break;
			default:
				break;
		}
		
	}
	
	
	void OpenGLPath::tessEnd( GLvoid* user ) {
//		OpenGLPath* me = (OpenGLPath*)user;
//		me->endOfTesselation();
		
//		printf("end\n");
	}
	
	
	void OpenGLPath::tessVertex( GLvoid* vertex, GLvoid* user ) {
		OpenGLPath* me = (OpenGLPath*)user;
		GLdouble* v = (GLdouble*)vertex;
		me->addVertexToDestroy( v );
		
		if ( me->primType() == GL_TRIANGLE_FAN ) {
			// break up fans and strips into triangles
			switch ( vertexCount_ ) {
				case 0:
					startVertex_[0] = v[0];
					startVertex_[1] = v[1];
					break;
				case 1:
					lastVertex_[0] = v[0];
					lastVertex_[1] = v[1];
					break;
					
				default:
					me->addVertex( startVertex_ );
					me->addVertex( lastVertex_ );
					me->addVertex( v );
					lastVertex_[0] = v[0];
					lastVertex_[1] = v[1];
					break;
			}
		} else if ( me->primType() == GL_TRIANGLES ) {
			me->addVertex( v );
		} else if ( me->primType() == GL_TRIANGLE_STRIP ) {
			switch ( vertexCount_ ) {
				case 0:
					me->addVertex( v );
					break;
				case 1:
					startVertex_[0] = v[0];
					startVertex_[1] = v[1];
					me->addVertex( v );
					break;
				case 2:
					lastVertex_[0] = v[0];
					lastVertex_[1] = v[1];
					me->addVertex( v );
					break;
	
				default:
					me->addVertex( startVertex_ );
					me->addVertex( lastVertex_ );
					me->addVertex( v );
					startVertex_[0] = lastVertex_[0];
					startVertex_[1] = lastVertex_[1];
					lastVertex_[0] = v[0];
					lastVertex_[1] = v[1];
					break;
			}
		}
		vertexCount_++;

//		printf("\tvert[%d]: %f, %f, %f\n", vertexCount_, v[0], v[1], v[2] );
	}
	void OpenGLPath::tessCombine( GLdouble coords[3], void *data[4],
							GLfloat weight[4], void **outData,
							void *polygonData ) {
		GLdouble* vertex = new GLdouble[3];
		vertex[0] = coords[0];
		vertex[1] = coords[1];
		vertex[2] = coords[2];		
		*outData = vertex;
//todo!!!		me->addVertexToDestroy( v );
//		printf("combine\n");
		
	}
	
	void OpenGLPath::tessError( GLenum errorCode ) {
		printf("tesselator error: [%d] %s\n", errorCode, gluErrorString( errorCode) );
	}
	
	OpenGLPath::~OpenGLPath() {
		if ( _fillTesseleator ) {
			gluDeleteTess( _fillTesseleator );
			_fillTesseleator = 0;
		}
		
		glDeleteBuffers( 1, &_fillVBO );
	}
	
	
	
	
}