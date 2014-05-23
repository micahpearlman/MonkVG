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
#include "glBatch.h"
#include <cassert>

extern "C" {
#include "vega/path_utils.h"
#include "vega/vg_state.h"

#include "vega/stroker.h"
};

namespace MonkVG {
	
	
	
	void OpenGLPath::clear( VGbitfield caps ) {
		IPath::clear( caps );
		
		_fillVertices.clear();
		_fillTessVertices.clear();
		_strokeSegments.clear();
		_strokeData.clear();
		_strokeVertices.clear();
		_strokeTessVertices.clear();

		// delete vbo buffers
		if ( _strokeVBO != -1 ) {
			GL->glDeleteBuffers( 1, &_strokeVBO );
			_strokeVBO = -1;
		}
		
		if ( _fillVBO != -1 ) {
			GL->glDeleteBuffers( 1, &_fillVBO );
			_fillVBO = -1;
		}
	}
	
	void OpenGLPath::buildFillIfDirty() {
		IPaint* currentFillPaint = IContext::instance().getFillPaint();
		if ( currentFillPaint != _fillPaintForPath ) {
			_fillPaintForPath = (OpenGLPaint*)currentFillPaint;
			_isFillDirty = true;
		}
		// only build the fill if dirty or we are in batch build mode
		if ( _isFillDirty || IContext::instance().currentBatch() ) {
			buildFill();
		}
		_isFillDirty = false;
	}
	
	void printMat44( float m[4][4] ) {
		printf("--\n");
		for ( int x = 0; x < 4; x++ ) {
			printf("%f\t%f\t%f\t%f\n", m[x][0], m[x][1], m[x][2], m[x][3]);			
		}
	}
	
	bool OpenGLPath::draw( VGbitfield paintModes ) {
		
		if ( paintModes == 0 ) 
			return false;
		
		CHECK_GL_ERROR;
		
		// get the native OpenGL context
		OpenGLContext& glContext = (MonkVG::OpenGLContext&)IContext::instance();
		
		if( paintModes & VG_FILL_PATH ) {	// build the fill polygons
			buildFillIfDirty();
		}

		if( paintModes & VG_STROKE_PATH && (_isStrokeDirty == true || IContext::instance().currentBatch())  ) {
			buildStroke();
			_isStrokeDirty = false;
		}
		
		endOfTesselation( paintModes );


		if ( glContext.currentBatch() ) {
			return true;		// creating a batch so bail from here
		}

		
		glContext.beginRender();
		
		GL->glEnableClientState( GL_VERTEX_ARRAY );
		GL->glDisableClientState( GL_COLOR_ARRAY );
		VGImageMode oldImageMode = glContext.getImageMode();

		// configure based on paint type
		if ( _fillPaintForPath && _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_COLOR ) {
			GL->glDisable(GL_TEXTURE_2D);
			GL->glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		} else if ( _fillPaintForPath && (_fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_2x3_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_2x3_GRADIENT) ) {
			GL->glEnable( GL_TEXTURE_2D );
			GL->glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            //GL->glColor4f(1, 1, 1, 1);  // HACKHACK: need to fix when drawing texture with GL_REPLACE we don't use the current glColor
			
			glContext.setImageMode( VG_DRAW_IMAGE_NORMAL );
			
		}
		

		if( (paintModes & VG_FILL_PATH) && _fillVBO != -1 && _fillPaintForPath) {
			// draw
			IContext::instance().fill();
			GL->glBindBuffer( GL_ARRAY_BUFFER, _fillVBO );
			if ( _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_COLOR ) {
				GL->glVertexPointer( 2, GL_FLOAT, sizeof(v2_t), 0 );
			} else if ( (_fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_2x3_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_2x3_GRADIENT) ) {
				_fillPaintForPath->getGradientImage()->bind();
				GL->glVertexPointer( 2, GL_FLOAT, sizeof(textured_vertex_t), (GLvoid*)offsetof(textured_vertex_t, v) );
				GL->glTexCoordPointer( 2, GL_FLOAT, sizeof(textured_vertex_t), (GLvoid*)offsetof(textured_vertex_t, uv) );
			}
			GL->glDrawArrays( GL_TRIANGLES, 0, _numberFillVertices );
			
			// unbind any textures being used
			if ( (_fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_2x3_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_2x3_GRADIENT) ) {
				_fillPaintForPath->getGradientImage()->unbind();
				glContext.setImageMode( oldImageMode );
				
				GL->glDisable(GL_TEXTURE_2D);
				GL->glDisableClientState( GL_TEXTURE_COORD_ARRAY );

			}
			
			// this is important to unbind the vbo when done
			GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}
		
		if ( (paintModes & VG_STROKE_PATH) && _strokeVBO != -1 ) {
			// draw
			IContext::instance().stroke();
			GL->glBindBuffer( GL_ARRAY_BUFFER, _strokeVBO );
			GL->glVertexPointer( 2, GL_FLOAT, sizeof(v2_t), 0 );
			GL->glDrawArrays( GL_TRIANGLES, 0, _numberStrokeVertices );
			GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );			
		}
		
		glContext.endRender();
		
		CHECK_GL_ERROR;
		
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
    
    static inline VGfloat calcQuadBezier1d( VGfloat start, VGfloat control, VGfloat end, VGfloat time ) {
        float inverseTime = 1.0f - time;
        return (powf(inverseTime, 2.0f) * start) + (2.0f * inverseTime * time * control) + (powf(time, 2.0f) * end);
    }
	
#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif
	// Given: Points (x0, y0) and (x1, y1) 
	// Return: TRUE if a solution exists, FALSE otherwise 
	//	Circle centers are written to (cx0, cy0) and (cx1, cy1) 
	static VGboolean findUnitCircles(VGfloat x0, VGfloat y0, VGfloat x1, VGfloat y1,
									 VGfloat *cx0, VGfloat *cy0, VGfloat *cx1, VGfloat *cy1) {								 
		
		// Compute differences and averages
		VGfloat dx = x0 - x1;
		VGfloat dy = y0 - y1;
		VGfloat xm = (x0 + x1)/2;
		VGfloat ym = (y0 + y1)/2; 
		VGfloat dsq, disc, s, sdx, sdy;
		// Solve for intersecting unit circles 
		dsq = dx*dx + dy*dy; 
		if (dsq == 0.0) 
			return VG_FALSE; // Points are coincident 
		disc = 1.0f/dsq - 1.0f/4.0f;
		if (disc < 0.0) 
			return VG_FALSE; // Points are too far apart 
		
		s = sqrt(disc); 
		sdx = s*dx; 
		sdy = s*dy;
		*cx0 = xm + sdy; 
		*cy0 = ym - sdx;
		*cx1 = xm - sdy;
		*cy1 = ym + sdx; 
		
		return VG_TRUE;
	}
	
	//Given: 
	//Return: 
	//Ellipse parameters rh, rv, rot (in degrees), endpoints (x0, y0) and (x1, y1) TRUE if a solution exists, FALSE otherwise. Ellipse centers are written to (cx0, cy0) and (cx1, cy1)
	
	static VGboolean findEllipses(VGfloat rh, VGfloat rv, VGfloat rot,
								  VGfloat x0, VGfloat y0, VGfloat x1, VGfloat y1, 
								  VGfloat *cx0, VGfloat *cy0, VGfloat *cx1, VGfloat *cy1) {
		VGfloat COS, SIN, x0p, y0p, x1p, y1p, pcx0, pcy0, pcx1, pcy1;
		// Convert rotation angle from degrees to radians 
		rot *= M_PI/180.0;
		// Pre-compute rotation matrix entries 
		COS = cos(rot); 
		SIN = sin(rot);
		// Transform (x0, y0) and (x1, y1) into unit space 
		// using (inverse) rotate, followed by (inverse) scale	
		x0p = (x0*COS + y0*SIN)/rh; 
		y0p = (-x0*SIN + y0*COS)/rv; 
		x1p = (x1*COS + y1*SIN)/rh; 
		y1p = (-x1*SIN + y1*COS)/rv;
		if (!findUnitCircles(x0p, y0p, x1p, y1p, &pcx0, &pcy0, &pcx1, &pcy1)) {
			return VG_FALSE;
		}
		// Transform back to original coordinate space 
		// using (forward) scale followed by (forward) rotate 
		pcx0 *= rh; 
		pcy0 *= rv; 
		pcx1 *= rh; 
		pcy1 *= rv;
		*cx0 = pcx0*COS - pcy0*SIN; 
		*cy0 = pcx0*SIN + pcy0*COS; 
		*cx1 = pcx1*COS - pcy1*SIN; 
		*cy1 = pcx1*SIN + pcy1*COS;
		
		return VG_TRUE;	
	}
	
	
	void OpenGLPath::buildGen(vector<VGubyte> &segments, vector<VGfloat> &fcoords) {
		_tessVertices->clear();
		_vertices->clear();

		// reset the bounds
		_minX = VG_MAX_FLOAT;
		_minY = VG_MAX_FLOAT;
		_width = -VG_MAX_FLOAT;
		_height = -VG_MAX_FLOAT;

		
		CHECK_GL_ERROR;
		
		GLUtesselator * tess = gluNewTess();
		gluTessCallback( tess, GLU_TESS_BEGIN_DATA, (GLvoid (APIENTRY *) ( )) &OpenGLPath::tessBegin );
		gluTessCallback( tess, GLU_TESS_END_DATA, (GLvoid (APIENTRY *) ( )) &OpenGLPath::tessEnd );
		gluTessCallback( tess, GLU_TESS_VERTEX_DATA, (GLvoid (APIENTRY *) ( )) &OpenGLPath::tessVertex );
		gluTessCallback( tess, GLU_TESS_COMBINE_DATA, (GLvoid (APIENTRY *) ( )) &OpenGLPath::tessCombine );
		gluTessCallback( tess, GLU_TESS_ERROR, (GLvoid (APIENTRY *)())&OpenGLPath::tessError );
		if( IContext::instance().getFillRule() == VG_EVEN_ODD ) {
			gluTessProperty( tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD ); 
		} else if( IContext::instance().getFillRule() == VG_NON_ZERO ) {
			gluTessProperty( tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO ); 
		}
		gluTessProperty( tess, GLU_TESS_TOLERANCE, 0.5f );
		
		gluTessBeginPolygon( tess, this );
		
		
		vector< VGfloat >::iterator coordsIter = fcoords.begin();
		VGbyte segment = VG_CLOSE_PATH;
		v3_t coords(0,0,0);
		v3_t prev(0,0,0);
		v3_t closeTo(0,0,0);
		int num_contours = 0;
		
		for ( vector< VGubyte >::iterator segmentIter = segments.begin(); segmentIter != segments.end(); segmentIter++ ) {
			segment = (*segmentIter);
			//int numCoords = segmentToNumCoordinates( static_cast<VGPathSegment>( segment ) );
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
			bool isRelative = segment & VG_RELATIVE;
			switch (segment >> 1) {
			case (VG_CLOSE_PATH >> 1):
			{
				GLdouble * l = addTessVertex( closeTo );
				gluTessVertex( tess, l, l );
				if ( num_contours ) {
					gluTessEndContour( tess );
					num_contours--;
				}

			} break;
			case (VG_MOVE_TO >> 1):
			{	
				if ( num_contours ) {
					gluTessEndContour( tess );
					num_contours--;
				}
					
				gluTessBeginContour( tess );
				num_contours++;
				closeTo.x = coords.x = *coordsIter; coordsIter++;
				closeTo.y = coords.y = *coordsIter; coordsIter++;
					
				GLdouble * l = addTessVertex( coords );
				gluTessVertex( tess, l, l );
					
			} break;
			case (VG_LINE_TO >> 1):
			{
				prev = coords;
				coords.x = *coordsIter; coordsIter++;
				coords.y = *coordsIter; coordsIter++;
				if ( isRelative ) {
					coords.x += prev.x;
					coords.y += prev.y;
				}
					
				GLdouble * l = addTessVertex( coords );
				gluTessVertex( tess, l, l );
			} break;
			case (VG_HLINE_TO >> 1):
			{
				prev = coords;
				coords.x = *coordsIter; coordsIter++;
				if ( isRelative ) {
					coords.x += prev.x;
				}
					
				GLdouble * l = addTessVertex( coords );
				gluTessVertex( tess, l, l );
			} break;
			case (VG_VLINE_TO >> 1):
			{
				prev = coords;
				coords.y = *coordsIter; coordsIter++;
				if ( isRelative ) {
					coords.y += prev.y;
				}
					
				GLdouble * l = addTessVertex( coords );
				gluTessVertex( tess, l, l );
			} break;
			case (VG_SCUBIC_TO >> 1): 
			{
				prev = coords;
				VGfloat cp2x = *coordsIter; coordsIter++;
				VGfloat cp2y = *coordsIter; coordsIter++;
				VGfloat p3x = *coordsIter; coordsIter++;
				VGfloat p3y = *coordsIter; coordsIter++;

					
				if ( isRelative ) {
					cp2x += prev.x;
					cp2y += prev.y;
					p3x += prev.x;
					p3y += prev.y;
				}
					
				VGfloat cp1x = 2.0f * cp2x - p3x;
				VGfloat cp1y = 2.0f * cp2y - p3y;

					
				VGfloat increment = 1.0f / IContext::instance().getTessellationIterations();
				//printf("\tcubic: ");
				for ( VGfloat t = increment; t < 1.0f + increment; t+=increment ) {
					v3_t c;
					c.x = calcCubicBezier1d( coords.x, cp1x, cp2x, p3x, t );
					c.y = calcCubicBezier1d( coords.y, cp1y, cp2y, p3y, t );
					c.z = 0;
					GLdouble * l = addTessVertex( c );
					gluTessVertex( tess, l, l );
					//	c.print();
				}
				//printf("\n");
				coords.x = p3x;
				coords.y = p3y;

			}
			break;
			case (VG_CUBIC_TO >> 1):
			{
				prev = coords;
				VGfloat cp1x = *coordsIter; coordsIter++;
				VGfloat cp1y = *coordsIter; coordsIter++;
				VGfloat cp2x = *coordsIter; coordsIter++;
				VGfloat cp2y = *coordsIter; coordsIter++;
				VGfloat p3x = *coordsIter; coordsIter++;
				VGfloat p3y = *coordsIter; coordsIter++;
					
				if ( isRelative ) {
					cp1x += prev.x;
					cp1y += prev.y;
					cp2x += prev.x;
					cp2y += prev.y;
					p3x += prev.x;
					p3y += prev.y;
				}
					
				VGfloat increment = 1.0f / IContext::instance().getTessellationIterations();
				//printf("\tcubic: ");
				for ( VGfloat t = increment; t < 1.0f + increment; t+=increment ) {
					v3_t c;
					c.x = calcCubicBezier1d( coords.x, cp1x, cp2x, p3x, t );
					c.y = calcCubicBezier1d( coords.y, cp1y, cp2y, p3y, t );
					c.z = 0;
					GLdouble * l = addTessVertex( c );
					gluTessVertex( tess, l, l );
					//	c.print();
				}
				//printf("\n");
				coords.x = p3x;
				coords.y = p3y;
					
			} break;
                    
			case (VG_QUAD_TO >> 1):
			{
				prev = coords;
				VGfloat cpx = *coordsIter; coordsIter++;
				VGfloat cpy = *coordsIter; coordsIter++;
				VGfloat px = *coordsIter; coordsIter++;
				VGfloat py = *coordsIter; coordsIter++;
					
				if ( isRelative ) {
					cpx += prev.x;
					cpy += prev.y;
					px += prev.x;
					py += prev.y;
				}
					
				VGfloat increment = 1.0f / IContext::instance().getTessellationIterations();
				for ( VGfloat t = increment; t < 1.0f + increment; t+=increment ) {
					v3_t c;
					c.x = calcQuadBezier1d( coords.x, cpx, px, t );
					c.y = calcQuadBezier1d( coords.y, cpy, py, t );
					c.z = 0;
					GLdouble * l = addTessVertex( c );
					gluTessVertex( tess, l, l );
				}
				coords.x = px;
				coords.y = py;
   
			} break;
				
			case (VG_SCCWARC_TO >> 1):
			case (VG_SCWARC_TO >> 1):
			case (VG_LCCWARC_TO >> 1):
			case (VG_LCWARC_TO >> 1):	
					
			{
				VGfloat rh = *coordsIter; coordsIter++;
				VGfloat rv = *coordsIter; coordsIter++;
				VGfloat rot = *coordsIter; coordsIter++;
				VGfloat cp1x = *coordsIter; coordsIter++;
				VGfloat cp1y = *coordsIter; coordsIter++;
				if ( isRelative ) {
					cp1x += prev.x;
					cp1y += prev.y;
				}
					
				// convert to Center Parameterization (see OpenVG Spec Apendix A)
				VGfloat cx0[2];
				VGfloat cx1[2];
				VGboolean success = findEllipses( rh, rv, rot,
								  coords.x, coords.y, cp1x, cp1y,
								  &cx0[0], &cx0[1], &cx1[0], &cx1[1] );
					
				if ( success ) {
					// see: http://en.wikipedia.org/wiki/Ellipse#Ellipses_in_computer_graphics 
					const int steps = IContext::instance().getTessellationIterations();
					VGfloat beta = 0;	// angle. todo
					VGfloat sinbeta = sinf( beta );
					VGfloat cosbeta = cosf( beta );
						
					// calculate the start and end angles
					v2_t center;
					center.x = cx0[0];
					center.y = cx0[1];
					v2_t norm[2];
					norm[0].x = center.x - coords.x;
					norm[0].y = center.y - coords.y;
					VGfloat inverse_len = 1.0f/sqrtf( (norm[0].x * norm[0].x) + (norm[0].y * norm[0].y) );
					norm[0].x *= inverse_len;
					norm[0].y *= inverse_len;
						
					norm[1].x = center.x - cp1x;
					norm[1].y = center.y - cp1y;
					inverse_len = 1.0f/sqrtf( (norm[1].x * norm[1].x) + (norm[1].y * norm[1].y) );
					norm[1].x *= inverse_len;
					norm[1].y *= inverse_len;
					VGfloat startAngle = degrees( acosf( -norm[0].x ) );
					VGfloat endAngle = degrees( acosf( -norm[1].x ) );
						
					VGfloat cross = norm[0].x;
						
					if ( cross >= 0  ) {
						startAngle = 360 - startAngle;
						endAngle = 360 - endAngle;
					}
					if ( startAngle > endAngle ) {
						VGfloat tmp = startAngle;
						startAngle = endAngle;
						endAngle = tmp;
						startAngle = startAngle - 90;
						endAngle = endAngle - 90;
					}
					for ( VGfloat g = startAngle; g < endAngle; g+=360/steps ) {
						v3_t c;
							
						VGfloat alpha = g * (M_PI / 180.0f);
						VGfloat sinalpha = sinf( alpha );
						VGfloat cosalpha = cosf( alpha );
						c.x = cx0[0] + (rh * cosalpha * cosbeta - rv * sinalpha * sinbeta);
						c.y = cx0[1] + (rh * cosalpha * sinbeta + rv * sinalpha * cosbeta);
						c.z = 0;
						GLdouble * l = addTessVertex( c );
						gluTessVertex( tess, l, l );
					}
				}
					
				coords.x = cp1x;
				coords.y = cp1y;
					
			} break;
					
			default:
				printf("unkwown command\n");
				break;
			}
		}	// foreach segment
		
		if ( num_contours ) {
			gluTessEndContour( tess );
			num_contours--;
		}
		
		assert(num_contours == 0);
		
		gluTessEndPolygon( tess );
		
		gluDeleteTess( tess );		

		// final calculation of the width and height
		_width = fabsf(_width - _minX);
		_height = fabsf(_height - _minY);
		
		CHECK_GL_ERROR;
	}

	void OpenGLPath::buildFill() {
		_tessVertices = &_fillTessVertices;
		_vertices = &_fillVertices;
		buildGen(_segments, _fcoords);
	}

	static void transform(vector<GLfloat> *dst, vector<GLfloat> &src, const Matrix33 &m) {
		for (vector<VGfloat>::iterator i = src.begin(); i != src.end();) {
			float v[2];
			v[0] = *i;
			i++;
			v[1] = *i;
			i++;
			float tv[2];
			affineTransform(tv, m, v);
			dst->push_back(tv[0]);
			dst->push_back(tv[1]);
		}
	}
	
	
	void OpenGLPath::buildStroke() {
		_strokeSegments.clear();
		_strokeData.clear();
		_strokeTessVertices.clear();
		_strokeVertices.clear();

                OpenGLContext& glContext = (MonkVG::OpenGLContext&)IContext::instance();

            
		struct vg_state state;
            
		state.stroke.line_width.f = glContext.getStrokeLineWidth();
		state.stroke.cap_style = glContext.getStrokeCapStyle();
		state.stroke.join_style = glContext.getStrokeJoinStyle();
		state.stroke.miter_limit.f = 4;
		state.stroke.dash_pattern_num = 0;

		struct stroker stroker;
		stroker.path = (struct path*)this;
		stroker_init(&stroker, &state);

		stroker_begin(&stroker);

		vector< VGfloat >::iterator c = _fcoords.begin();
		VGfloat x0 = 0;
		VGfloat y0 = 0;
		VGfloat x = 0;
		VGfloat y = 0;
		VGfloat px1, py1, px2, py2;

		for ( vector< VGubyte >::iterator s = _segments.begin(); s != _segments.end(); s++ ) {
			VGbyte segment = (*s);
			bool isRelative = segment & VG_RELATIVE;
			switch (segment >> 1) {
			case (VG_CLOSE_PATH >> 1):
				stroker_line_to(&stroker, x0, y0);
				break;
			case (VG_LINE_TO >> 1):
				x = *c++;
				y = *c++;
				stroker_line_to(&stroker, x, y);
				break;
			case (VG_MOVE_TO >> 1):
				x0 = x = *c++;
				y0 = y = *c++;
				stroker_move_to(&stroker, x, y);
				break;
			case (VG_CUBIC_TO >> 1):
				px1 = *c++;
				py1 = *c++;
				px2 = *c++;
				py2 = *c++;
				x = *c++;
				y = *c++;
				stroker_curve_to(&stroker, px1, py1, px2, py2, x, y);
				break;	
			default:
				printf("unkwown command\n");
				assert(0);
				break;
			}
		}
		stroker_end(&stroker);

		stroker_cleanup(&stroker);


		float mx = _minX;
		float my = _minY;
		float w = _width;
		float h = _height;
		_tessVertices = &_strokeTessVertices;
		_vertices = &_strokeVertices;

		VGFillRule fr = glContext.getFillRule();
		glContext.setFillRule(VG_NON_ZERO);
		buildGen(_strokeSegments, _strokeData);
		glContext.setFillRule(fr);
		_minX = mx;
		_minY = my;
		_width = w;
		_height = h;
	}
	
	void OpenGLPath::endOfTesselation( VGbitfield paintModes ) {
		
		/// build fill vbo
		// TODO: BUGBUG: if in batch mode don't build the VBO!
		if ( _fillVertices.size() > 0 ) {
			if ( _fillVBO != -1 ) {
				GL->glDeleteBuffers( 1, &_fillVBO );
				_fillVBO = -1;
			}
			
			GL->glGenBuffers( 1, &_fillVBO );
			GL->glBindBuffer( GL_ARRAY_BUFFER, _fillVBO );
			if ( _fillPaintForPath && _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_COLOR ) {
				GL->glBufferData( GL_ARRAY_BUFFER, _fillVertices.size() * sizeof(float), &_fillVertices[0], GL_STATIC_DRAW );
			} else if ( _fillPaintForPath && (_fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_RADIAL_2x3_GRADIENT || _fillPaintForPath->getPaintType() == VG_PAINT_TYPE_LINEAR_2x3_GRADIENT) ) {
				// setup the paints linear gradient
				_fillPaintForPath->buildGradientImage( _width, _height );

				// build up the textured vertex
				vector<textured_vertex_t> texturedVertices;
				for ( vector<float>::const_iterator it = _fillVertices.begin(); it != _fillVertices.end(); it++ ) {
					textured_vertex_t v;
					v.v[0] = *it;
					it++;
					v.v[1] = *it;
					v.uv[0] = fabsf(v.v[0] - _minX) / _width;
					v.uv[1] = fabsf( v.v[1] - _minY ) / _height;
					texturedVertices.push_back( v );
				}
				
				GL->glBufferData( GL_ARRAY_BUFFER, texturedVertices.size() * sizeof(textured_vertex_t), &texturedVertices[0], GL_STATIC_DRAW );
			}

			_numberFillVertices = (int)_fillVertices.size()/2;
			_fillTessVertices.clear();
			_fillVertices.clear();
		} 
		
		/// build stroke vbo 
		if ( _strokeVertices.size() > 0 ) {
			// build the vertex buffer object VBO
			if ( _strokeVBO != -1 ) {
				GL->glDeleteBuffers( 1, &_strokeVBO );
				_strokeVBO = -1;
			}
			
			GL->glGenBuffers( 1, &_strokeVBO );
			GL->glBindBuffer( GL_ARRAY_BUFFER, _strokeVBO );

			GL->glBufferData( GL_ARRAY_BUFFER, _strokeVertices.size() * sizeof(float), &_strokeVertices[0], GL_STATIC_DRAW );
			_numberStrokeVertices = (int)_strokeVertices.size() / 2;
			_strokeTessVertices.clear();
			_strokeVertices.clear();
			
		}
		
		OpenGLBatch* glBatch = (OpenGLBatch*)IContext::instance().currentBatch();
		if( glBatch ) {	// if in batch mode update the current batch
			glBatch->addPathVertexData( &_fillVertices[0], _fillVertices.size()/2, 
						    &_strokeVertices[0], _strokeVertices.size()/2, 
						    paintModes );

		}
	}
	
	static GLdouble startVertex_[2];
	static GLdouble lastVertex_[2];
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
		
		//printf("end\n");
	}
	
	
	void OpenGLPath::tessVertex( GLvoid* vertex, GLvoid* user ) {
		OpenGLPath* me = (OpenGLPath*)user;
		GLdouble* v = (GLdouble*)vertex;
		
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
		
				//printf("\tvert[%d]: %f, %f, %f\n", vertexCount_, v[0], v[1], v[2] );
	}
	void OpenGLPath::tessCombine( GLdouble coords[3], void *data[4],
								 GLfloat weight[4], void **outData,
								 void *polygonData ) {

		OpenGLPath* me = (OpenGLPath*)polygonData;
		*outData = me->addTessVertex( coords );
		
	}
	
	void OpenGLPath::tessError( GLenum errorCode ) {
		printf("tesselator error: [%d] %s\n", errorCode, gluErrorString( errorCode) );
	}
	
	OpenGLPath::~OpenGLPath() {
		clear(0);
	}

	void OpenGLPath::appendStrokeData(VGint nseg, const VGubyte * segments, const void * data) {
		_strokeSegments.insert(_strokeSegments.cend(), &segments[0], &segments[nseg]);
		VGint n = num_elements_for_segments(segments, nseg);
		GLfloat * fdata = (GLfloat *)data;
		_strokeData.insert(_strokeData.cend(), &fdata[0], &fdata[n]);
	}
	
extern "C" {

void path_append_data(struct path *p,
                      VGint numSegments,
                      const VGubyte * pathSegments,
                      const void * pathData){
	OpenGLPath * gp = (OpenGLPath *)p;
	gp->appendStrokeData(numSegments, pathSegments, pathData);
}
struct path *path_create(VGPathDatatype dt, VGfloat scale, VGfloat bias,
                         VGint segmentCapacityHint,
                         VGint coordCapacityHint,
                         VGbitfield capabilities){
	assert(0);
}

void path_move_to(struct path *p, float x, float y) {
	assert(0);
}
		
void path_line_to(struct path *p, float x, float y) {
	assert(0);
}

void path_cubic_to(struct path *p, float px1, float py1,
                   float px2, float py2,
                   float x, float y) {
	assert(0);
}

}

};
