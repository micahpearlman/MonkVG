//
//  mkImage.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __mkImage_h__
#define __mkImage_h__

#include "mkBaseObject.h"

namespace MonkVG {
	
	class IImage : public BaseObject {
	public:
		
		IImage( VGImageFormat format,
			   VGint width, VGint height,
			   VGbitfield allowedQuality )
		:	BaseObject()
		,	_format( format )
		,	_width( width )
		,	_height( height )
		,	_allowedQuality( allowedQuality )
		{}
		
		inline BaseObject::Type getType() const {
			return BaseObject::kImageType;
		}
		
		virtual void draw() = 0;
		
		virtual void drawSubRect( VGint ox, VGint oy, VGint w, VGint h, VGbitfield paintModes ) = 0;
		
		//// parameter accessors/mutators ////
		virtual VGint getParameteri( const VGint p ) const;
		virtual VGfloat getParameterf( const VGint f ) const;
		virtual void getParameterfv( const VGint p, VGfloat *fv ) const;
		virtual void setParameter( const VGint p, const VGfloat f );
		virtual void setParameter( const VGint p, const VGint i );
		virtual void setParameter( const VGint p, const VGfloat* fv );
		
		virtual void setSubData( const void * data, VGint dataStride,
						   VGImageFormat dataFormat,
						   VGint x, VGint y, VGint width, VGint height ) = 0;
	protected:
		
		VGImageFormat	_format;
		VGint			_width;
		VGint			_height;
		VGbitfield		_allowedQuality;
		
	};
	
}

#endif // __mkImage_h__
