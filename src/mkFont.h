//
//  mkFont.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/29/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __mkFont_h__
#define __mkFont_h__

#include "mkBaseObject.h"
#include "mkImage.h"
#include "mkPath.h"

#include <unordered_map>
#include <memory>

namespace MonkVG {
	
	class IFont : public BaseObject {
	public:
		
		IFont(  )
		:	BaseObject()
		{}
		
		inline BaseObject::Type getType() const {
			return BaseObject::kFontType;
		}
		
		virtual void drawGlyph( VGuint index, VGfloat adj_x, VGfloat adj_y, VGbitfield paintModes );
		
		//// parameter accessors/mutators ////
		virtual VGint getParameteri( const VGint p ) const;
		virtual VGfloat getParameterf( const VGint f ) const;
		virtual void getParameterfv( const VGint p, VGfloat *fv ) const;
		virtual void setParameter( const VGint p, const VGfloat f );
		virtual void setParameter( const VGint p, const VGint i );
		virtual void setParameter( const VGint p, const VGfloat* fv, const VGint cnt );
		
		virtual void addGlyphImage( VGuint index_, IImage* image_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] );
		virtual void addGlyphPath( VGuint index_, IPath* path_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] );
		
	protected:
		struct Glyph {
			VGuint				index;
			BaseObject::Type	type;	// type can be either: kPathType or kImageType
			VGfloat				glyphOrigin[2];
			VGfloat				escapement[2];
			
			Glyph( VGuint index_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] ) : index( index_ ), type( BaseObject::kMAXIMUM_TYPE )
			{
				glyphOrigin[0] = glyphOrigin_[0];
				glyphOrigin[1] = glyphOrigin_[1];
				
				escapement[0] = escapement_[0];
				escapement[1] = escapement_[1];
			}
			
			virtual void draw( VGbitfield paintModes, VGfloat adj_x, VGfloat adj_y ) = 0;
		};
		
		struct GlyphImage : public Glyph {
			IImage*				image;
			
			GlyphImage( VGuint index_, IImage* image_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] )
			:	Glyph( index_, glyphOrigin_, escapement_ )
			,	image( image_ )
			{
				type = BaseObject::kImageType;
			}
			
			virtual void draw( VGbitfield paintModes, VGfloat adj_x, VGfloat adj_y ); 
		};
		
		struct GlyphPath : public Glyph {
			IPath*				path;
			
			GlyphPath( VGuint index_, IPath* path_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] )
			:	Glyph( index_, glyphOrigin_, escapement_ )
			,	path( path_ )
			{
				type = BaseObject::kPathType;
			}
			
			virtual void draw( VGbitfield paintModes, VGfloat adj_x, VGfloat adj_y ) {
				path->draw( paintModes );
			}

		};
		
		std::unordered_map<VGuint, std::shared_ptr<Glyph> >	_glyphs;
	};
	
}

#endif // __mkFont_h__
