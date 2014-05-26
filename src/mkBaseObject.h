/*
 *  mkBaseObject.h
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkBaseObject_h__
#define __mkBaseObject_h__

#include "MonkVG/openvg.h"

namespace MonkVG {
	
	class BaseObject {
	public:
	
		//// object types ////
		enum Type {
			kPathType,
			kPaintType,
			kImageType,
			kMaskLayerType,
			kFontType,
			kBatchType,
			
			kMAXIMUM_TYPE
		};
		
		virtual BaseObject::Type getType() const = 0;
		
		//// parameter accessors/mutators ////
		virtual VGint getParameteri( const VGint p ) const = 0;
		virtual VGfloat getParameterf( const VGint f ) const = 0;
		virtual void getParameterfv( const VGint p, VGfloat *fv ) const = 0;
		virtual void setParameter( const VGint p, const VGfloat f ) = 0;
		virtual void setParameter( const VGint p, const VGint i ) = 0;
		virtual void setParameter( const VGint p, const VGfloat* fv, const VGint cnt ) = 0;
	};

	void SetError( const VGErrorCode e );
}


#endif
