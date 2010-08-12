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

namespace MonkVG {
	
	bool OpenGLPath::draw( VGbitfield paintModes ) {
	
		// get the native OpenGL context
		OpenGLContext& glContext = (MonkVG::OpenGLContext&)IContext::instance();

		if( paintModes & VG_FILL_PATH && _fillTesseleator == 0 ) {	// build the fill polygons
			_fillTesseleator = gluNewTess();
			gluTessCallback( _fillTesseleator, GLU_TESS_BEGIN_DATA, (GLvoid (*) ( )) &OpenGLPath::tessBegin );
			gluTessCallback( _fillTesseleator, GLU_TESS_END_DATA, (GLvoid (*) ( )) &OpenGLPath::tessEnd );
			gluTessCallback( _fillTesseleator, GLU_TESS_VERTEX_DATA, (GLvoid (*) ( )) &OpenGLPath::tessVertex );
			gluTessCallback( _fillTesseleator, GLU_TESS_COMBINE, (GLvoid (*) ( )) &OpenGLPath::tessCombine );			
			//gluTessProperty( _fillTesseleator, GLU_TESS_BOUNDARY_ONLY, GL_FALSE ); 
			gluTessProperty( _fillTesseleator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD ); 
			
			gluTessBeginPolygon( _fillTesseleator, this );
			gluTessBeginContour( _fillTesseleator );
			
			vector< VGfloat >::iterator coordsIter = _fcoords->begin();
			int numCoords = 0;
			VGbyte segment = VG_CLOSE_PATH;
			GLdouble coords[3] = {0,0,0};
			GLdouble closeTo[3] = {0,0,0};
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
						// do not think this is necessary for the tesselator						gluTessVertex( _fillTesseleator, c, c );
					} break;
					case (VG_MOVE_TO >> 1):
					{	
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
						//					VGfloat cp1x = *coordsIter; coordsIter++;
						//					VGfloat cp1y = *coordsIter; coordsIter++;
						//					VGfloat cp2x = *coordsIter; coordsIter++;
						//					VGfloat cp2y = *coordsIter; coordsIter++;
						//					coords[0] = *coordsIter; coordsIter++;
						//					coords[1] = *coordsIter; coordsIter++;
					}
						break;	
					default:
						break;
				}
			}	// foreach segment
			
			gluTessEndContour( _fillTesseleator );
			gluTessEndPolygon( _fillTesseleator );

		}
		if( paintModes & VG_STROKE_PATH )
		{
		}
		
		glContext.beginRender();
		
		
		
		Matrix33 active = *IContext::instance().getActiveMatrix();
		// a	b	0
		// c	d	0
		// tx	ty	1
		active.transpose();		// NOTE:  have to transpose.  Maybe should set up the matrices already as transposed?
		
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
		//todo: translation
		glPushMatrix();
		glLoadMatrixf( &mat44[0][0] );
		if( paintModes & VG_FILL_PATH ) {
			
			// draw
			glColor4f(1, 1, 1, 1);
			glBindBuffer( GL_ARRAY_BUFFER, _fillVBO );
			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer( 2, GL_FLOAT, sizeof(float) * 2, 0 );
			glDrawArrays( GL_TRIANGLES, 0, _numberVertices );

		}
		
		glContext.endRender();
		glPopMatrix();
		
		return true;
	}
	
	
	void OpenGLPath::endOfTesselation() {
		glGenBuffers( 1, &_fillVBO );
		glBindBuffer( GL_ARRAY_BUFFER, _fillVBO );
		glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(float) * 2, &_vertices[0], GL_STATIC_DRAW );
		_numberVertices = _vertices.size()/2;
		for (list<GLdouble*>::iterator iter = _verticesToDestroy.begin(); iter != _verticesToDestroy.end(); iter++ ) {
			delete [] *(iter);
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
				printf( "begin(GL_TRIANGLES)\n" );
				break;
			case GL_TRIANGLE_FAN:
				printf( "begin(GL_TRIANGLE_FAN)\n" );
				break;
			case GL_TRIANGLE_STRIP:
				printf( "begin(GL_TRIANGLE_STRIP)\n" );
				break;
			case GL_LINE_LOOP:
				printf( "begin(GL_LINE_LOOP)\n" );
				break;
			default:
				break;
		}
		
	}
	
	
	void OpenGLPath::tessEnd( GLvoid* user ) {
		OpenGLPath* me = (OpenGLPath*)user;
		me->endOfTesselation();
		
		printf("end\n");
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
			
			vertexCount_++;
		}
		printf("vert: %f, %f, %f\n", v[0], v[1], v[2] );
	}
	void OpenGLPath::tessCombine( GLdouble coords[3], void *data[4],
							GLfloat weight[4], void **outData,
							void *polygonData ) {
		
		printf("combine\n");
		
	}
	
	
	OpenGLPath::~OpenGLPath() {
		if ( _fillTesseleator ) {
			gluDeleteTess( _fillTesseleator );
			_fillTesseleator = 0;
		}
		
		glDeleteBuffers( 1, &_fillVBO );
	}
	
	
	
	
}