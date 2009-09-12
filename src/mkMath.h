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

namespace MonkVG {
	
	static inline VGfloat radians (VGfloat degrees) {return degrees * M_PI/180.0f;}	

//	[ sx	shx	tx
//	 shy	sy	ty
//	 0		0	1 ]
//	sx and sy define scaling in the x and y directions, respectively;
//	shx and shy define shearing in the x and y directions, respectively;
//	tx and ty define translation in the x and y directions, respectively.

	class Matrix33 {
	public:
	
		Matrix33() {
			
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
					set( i, k, get( i, k ) );
						
		}
		inline void transpose( ) {
			for( int i = 0; i < 3; i++ )
				for( int k = 0; k < 3; k++ )
					set( i, k, get( k, i ) );
		}
		inline void multiply( const Matrix33& m ) {
			Matrix33 tmp;
			for( int j = 0; j < 3; j++ ) 
				for( int i = 0; i < 3; i++ ) 
					tmp.set( j, i, get( j, 0 ) * m.get( 0, i )
									+ get( j, 1 ) * m.get( 1, i )
									+ get( j, 2 ) * m.get( 2, i ) );
			copy( tmp );
		}
		
		inline void setScale( VGfloat sx, VGfloat sy ) {
			set( 0, 0, sx ); set( 1, 1, sy );
		}
		inline void setTranslate( VGfloat tx, VGfloat ty ) {
			set( 0, 2, tx ); set( 1, 2, ty );
		}
		inline void setRotate( VGfloat a ) {
			set( 0, 0, cos( a ) );	set( 0, 1, -sin( a ) );
			set( 1, 0, sin( a ) );	set( 1, 1, cos( a ) );			
		}
		
		
	private:
	
		VGfloat _mat[3][3];
	};
}
#endif // __mkMath_h__