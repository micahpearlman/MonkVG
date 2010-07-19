/*
 *  qzContext.cpp
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/2/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "qzContext.h"
#include "qzPath.h"
#include "qzPaint.h"
#include <UIKit/UIKit.h> // for UIGraphicsGetCurrentContext and why this is a .mm file

namespace MonkVG {
	
	QuartzContext::QuartzContext()
		:	IContext()
	{
	}
	
	
	bool QuartzContext::Initialize() {
	
		// get the UIKit currently active context
		_context = UIGraphicsGetCurrentContext();

		return true;
	}
	
	
	bool QuartzContext::Terminate() {
		return true;
	}
	
	CGContextRef QuartzContext::getNativeContext() {
		if( _context == nil )
		{
			_context = UIGraphicsGetCurrentContext();
			CGContextSetShouldAntialias ( _context, NO );
			CGContextSetInterpolationQuality ( _context, kCGInterpolationLow );
		}
		
		return _context;
	}
	
	IPath* QuartzContext::createPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) {
	
		QuartzPath *path = new QuartzPath(pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities  &= VG_PATH_CAPABILITY_ALL);
		if( path == 0 )
			setError( VG_OUT_OF_MEMORY_ERROR );
	
		return (IPath*)path;
	}
	
	void QuartzContext::destroyPath( IPath* path ) {
		delete (QuartzPath*)path;
	}
	
	IPaint* QuartzContext::createPaint() {
		QuartzPaint *paint = new QuartzPaint();
		if( paint == 0 )
			setError( VG_OUT_OF_MEMORY_ERROR );
		return (IPaint*)paint;
	}
	
	void QuartzContext::stroke() {
		CGContextSetLineWidth( getNativeContext(), getStrokeLineWidth() );
		CGContextSetStrokeColor( getNativeContext(), getStrokePaint()->getPaintColor() );
	}
	
	void QuartzContext::fill() {
		CGContextFillPath ( getNativeContext() );
		if( getFillPaint() )
			CGContextSetFillColor( getNativeContext(), getFillPaint()->getPaintColor() );
	}
	
	void QuartzContext::clear(VGint x, VGint y, VGint width, VGint height) {
	
		// ??? fill color
		CGContextClearRect ( getNativeContext(), CGRectMake( x, y, width, height ) );
	}
	
	void QuartzContext::setIdentity() {
		Matrix33* active = getActiveMatrix();
		active->setIdentity();
//		if( _context_stack.size() > 1 ) {
//			CGContextRestoreGState( getNativeContext() );
//			_context_stack.pop(); 
//		}
//			
//		CGContextSaveGState ( getNativeContext() );
//		_context_stack.push( true );

//		CGAffineTransform ctm = CGContextGetCTM( getNativeContext() );
//		if( CGAffineTransformIsIdentity( ctm ) == false ) {
//			CGAffineTransform inverse = CGAffineTransformInvert ( ctm );
//			CGContextConcatCTM( getNativeContext(), inverse );
//		}

	}
	void QuartzContext::transform() {
		// a	b	0
		// c	d	0
		// tx	ty	1
//		Matrix33 active = *getActiveMatrix();
//		active.transpose();
//		
//		CGAffineTransform transform = CGAffineTransformMake ( active.get( 0, 0 ), active.get( 0, 1 ),
//												  active.get( 1, 0 ), active.get( 1, 1 ),
//												  active.get( 2, 0 ), active.get( 2, 1 ) );
//												  
//		CGContextConcatCTM( getNativeContext(), transform );
	}

	void QuartzContext::scale( VGfloat sx, VGfloat sy ) {
		//CGContextScaleCTM( getNativeContext(), sx, sy );
		Matrix33* active = getActiveMatrix();
		Matrix33 scale;
		scale.setScale( sx, sy );
		active->multiply( scale );
	}
	void QuartzContext::translate( VGfloat x, VGfloat y ) {
		//CGContextTranslateCTM( getNativeContext(), x, y );
		Matrix33* active = getActiveMatrix();
		Matrix33 translate;
		translate.setTranslate( x, y );
		active->multiply( translate );
	}
	void QuartzContext::rotate( VGfloat angle ) {
		//CGContextRotateCTM( getNativeContext(), radians( angle ) );
		Matrix33* active = getActiveMatrix();
		Matrix33 rotate;
		rotate.setRotate( radians( angle ) );
		active->multiply( rotate );
	}
	
	
	//// singleton implementation ////
	static QuartzContext g_context;
	IContext& IContext::instance()
	{
		return g_context;
	}
	
}