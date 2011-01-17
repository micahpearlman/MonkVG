/*
 *  mkMath.h
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/11/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkMath_h__
#define __mkMath_h__

#include "VG/openvg.h"
#include <cmath>
#include "mkCommon.h"

namespace MonkVG {
	
	static inline VGfloat radians (VGfloat degrees) {return degrees * (M_PI/180.0f);}	
	static inline VGfloat degrees (VGfloat radians) {return radians * (180.0f/M_PI);}

//	[ sx	shx	tx
//	 shy	sy	ty
//	 0		0	1 ]
//	sx and sy define scaling in the x and y directions, respectively;
//	shx and shy define shearing in the x and y directions, respectively;
//	tx and ty define translation in the x and y directions, respectively.

	class Matrix33 {
	public:
	
		Matrix33() {
			//setIdentity();
		}
		
		Matrix33( const Matrix33 &m ) {
			this->copy( m );
		}
		
		inline void set( int row, int col, VGfloat v ) {
			_mat[row][col] = v; 
		}
		
		inline VGfloat get( int row, int col ) const {
			return _mat[row][col];
		}
		
		inline void setIdentity() {
			for ( int i = 0; i < 3; i++ ) {
				for ( int k = 0; k < 3; k++ ) {
					if( i == k ) 
						set( i, k, 1.0f );
					else 
						set( i, k, 0.0f );
				}
			}
		}
		inline void copy( const Matrix33& m ) {
			for( int i = 0; i < 3; i++ )
				for( int k = 0; k < 3; k++ )
					set( i, k, m.get( i, k ) );
						
		}
		inline void transpose( ) {
			Matrix33 tmp;
			for( int i = 0; i < 3; i++ )
				for( int k = 0; k < 3; k++ )
					tmp.set( i, k, get( k, i ) );
			copy( tmp );
		}
		inline void postMultiply( const Matrix33& m ) {
			Matrix33 tmp;
			for( int j = 0; j < 3; j++ ) 
				for( int i = 0; i < 3; i++ ) 
					tmp.set( j, i, get( j, 0 ) * m.get( 0, i )
									+ get( j, 1 ) * m.get( 1, i )
									+ get( j, 2 ) * m.get( 2, i ) );
			copy( tmp );
		}

		inline void preMultiply( const Matrix33& m ) {
			Matrix33 tmp;
			for( int j = 0; j < 3; j++ ) 
				for( int i = 0; i < 3; i++ ) 
					tmp.set( j, i, m.get( j, 0 ) * get( 0, i )
							+ m.get( j, 1 ) * get( 1, i )
							+ m.get( j, 2 ) * get( 2, i ) );
			copy( tmp );
		}
		
		inline void setScale( VGfloat sx, VGfloat sy ) {
			setIdentity();
			set( 0, 0, sx ); set( 1, 1, sy );
		}
		inline void setTranslate( VGfloat tx, VGfloat ty ) {
			setIdentity();
			set( 0, 2, tx ); set( 1, 2, ty );
		}
		inline void setRotate( VGfloat a ) {
			setIdentity();
			set( 0, 0, cosf( a ) );	set( 0, 1, sinf( a ) );
			set( 1, 0, -sinf( a ) );	set( 1, 1, cosf( a ) );
		}
		inline void addTranslate( VGfloat tx, VGfloat ty ) {
			set( 0, 2, get(0, 2) + tx);
			set( 1, 2, get(1, 2) + ty);
		}
		inline void getTranslate( VGfloat& tx, VGfloat& ty ) {
			tx = get( 0, 2 );
			ty = get( 1, 2 );
		}
		
	private:
	
		VGfloat _mat[3][3];
	};
}

#define RI_ASSERT(_) 

static inline int RI_ISNAN(float a) {
    return (a!=a)?1:0;
}

static inline float   RI_MAX(float a, float b)                            { return (a > b) ? a : b; }
static inline float   RI_MIN(float a, float b)                            { return (a < b) ? a : b; }
static inline float   RI_CLAMP(float a, float l, float h)       { if(RI_ISNAN(a)) return l; RI_ASSERT(l <= h); return (a < l) ? l : (a > h) ? h : a; }
static inline float   RI_SQR(float a)                                                       { return a * a; }
static inline float   RI_MOD(float a, float b){
	if(RI_ISNAN(a) || RI_ISNAN(b))
		return 0.0f;
    
	RI_ASSERT(b >= 0.0f);
	
	if(b == 0.0f)
		return 0.0f;
    
	float f = (float)fmod(a, b);
	
	if(f < 0.0f)
		f += b;
	RI_ASSERT(f >= 0.0f && f <= b);
	return f;
}

static inline int RI_INT_MAX(int a, int b)                      { return (a > b) ? a : b; }
static inline int RI_INT_MIN(int a, int b)                      { return (a < b) ? a : b; }
static inline uint16_t RI_UINT16_MIN(uint16_t a,uint16_t b) { return (a < b) ? a : b; } 
static inline uint32_t RI_UINT32_MIN(uint32_t a, uint32_t b)                    { return (a < b) ? a : b; }
static inline int RI_INT_MOD(int a, int b)                      { RI_ASSERT(b >= 0); if(!b) return 0; int i = a % b; if(i < 0) i += b; RI_ASSERT(i >= 0 && i < b); return i; }
static inline int RI_INT_CLAMP(int a, int l, int h)     { RI_ASSERT(l <= h); return (a < l) ? l : (a > h) ? h : a; }

static inline int RI_FLOOR_TO_INT(float value){
	if(value<0)
		return floor(value);
    
	return value;
}
#endif // __mkMath_h__