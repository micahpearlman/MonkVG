//
//  mkBatch.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/27/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __mkBatch_h__
#define __mkBatch_h__

#include <stdlib.h>
#include "mkBaseObject.h"

namespace MonkVG {
	
	class IBatch : public BaseObject {
	public:
		
		IBatch()
		:	BaseObject()
		{}
		
		inline BaseObject::Type getType() const {
			return BaseObject::kBatchType;
		}
		
		//// parameter accessors/mutators ////
		virtual VGint getParameteri( const VGint p ) const;
		virtual VGfloat getParameterf( const VGint f ) const;
		virtual void getParameterfv( const VGint p, VGfloat *fv ) const;
		virtual void setParameter( const VGint p, const VGfloat f );
		virtual void setParameter( const VGint p, const VGint i );
		virtual void setParameter( const VGint p, const VGfloat* fv, const VGint cnt );
		
		virtual void draw() = 0;
        virtual void dump( void **vertices, size_t *size ) = 0;
		virtual void finalize() = 0;
		
		virtual ~IBatch() {}
		
	private:
		
		
	};
	
}

#endif // __mkBatch_h__
