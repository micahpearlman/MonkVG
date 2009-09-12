/*
 *  mkPaint.h
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/3/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkPaint_h__
#define __mkPaint_h__

#include "mkBaseObject.h"

namespace MonkVG {
	
	class IPaint : public BaseObject {
	public:
	
		IPaint()
			:	BaseObject()
		{}
	
		inline BaseObject::Type getType() const {
			return BaseObject::kPaintType;
		}
		
		//// parameter accessors/mutators ////
		virtual VGint getParameteri( const VGint p ) const;
		virtual VGfloat getParameterf( const VGint f ) const;
		virtual void getParameterfv( const VGint p, VGfloat *fv ) const;
		virtual void setParameter( const VGint p, const VGfloat f );
		virtual void setParameter( const VGint p, const VGint i );
		virtual void setParameter( const VGint p, const VGfloat* fv );
		
		const VGfloat* getPaintColor() const {
			return _paintColor;
		}
		
	private:
	
		VGPaintType				_paintType;
		VGfloat					_paintColor[4];
		VGColorRampSpreadMode	_colorRampSpreadMode;
		//	VGfloat					_colorRampStops[4];
		VGboolean				_colorRampPremultiplied;
		VGfloat					_paintLinearGradient[4];
		VGfloat					_paintRadialGradient[5];
		VGTilingMode			_patternTilingMode;
		//	Image*					m_pattern;
		
	};
	
}
#endif // __mkPaint_h__