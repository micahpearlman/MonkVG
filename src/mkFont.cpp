//
//  mkFont.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/29/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include "mkFont.h"
#include "mkContext.h"

namespace MonkVG {	// Internal Implementation
	VGint IFont::getParameteri( const VGint p ) const {
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	VGfloat IFont::getParameterf( const VGint p ) const {
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	void IFont::getParameterfv( const VGint p, VGfloat *fv ) const {
		switch (p) {
				
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
		
	}
	
	void IFont::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IFont::setParameter( const VGint p, const VGfloat v ) 
	{
		switch (p) {
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IFont::setParameter( const VGint p, const VGfloat* fv ) {
		switch (p) {
				
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	
	void IFont::addGlyphImage( VGuint index_, IImage* image_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] ) {
		_glyphs[index_] = new IFont::GlyphImage( index_, image_, glyphOrigin_, escapement_ );
	}
	void IFont::addGlyphPath( VGuint index_, IPath* path_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] ) {
		_glyphs[index_] = new IFont::GlyphPath( index_, path_, glyphOrigin_, escapement_ );
	}
	
	void IFont::drawGlyph( VGuint index, VGbitfield paintModes ) {
		boost::ptr_map<VGuint, Glyph*>::iterator it =  _glyphs.find( index );
		if ( it != _glyphs.end() ) {
			Glyph* glyph = *it->second;
			glyph->draw( paintModes );
		}
	}

	
}


///// OpenVG API Implementation /////

using namespace MonkVG;

VG_API_CALL VGFont VG_API_ENTRY vgCreateFont(VGint glyphCapacityHint) VG_API_EXIT {
	return (VGFont)IContext::instance().createFont();
}
VG_API_CALL void VG_API_ENTRY vgDestroyFont(VGFont font) VG_API_EXIT {
	IContext::instance().destroyFont( (IFont*)font );
}
VG_API_CALL void VG_API_ENTRY vgSetGlyphToPath(VGFont font,
											   VGuint glyphIndex,
											   VGPath path,
											   VGboolean isHinted,
											   VGfloat glyphOrigin [2],
											   VGfloat escapement[2]) VG_API_EXIT {
	if ( !font ) 
		return;
	
	IFont* f = (IFont*)font;
	f->addGlyphPath( glyphIndex, (IPath*)path, glyphOrigin, escapement );
	
}
VG_API_CALL void VG_API_ENTRY vgSetGlyphToImage(VGFont font,
												VGuint glyphIndex,
												VGImage image,
												VGfloat glyphOrigin [2],
												VGfloat escapement[2]) VG_API_EXIT {
	if ( !font ) 
		return;
	
	IFont* f = (IFont*)font;
	f->addGlyphImage( glyphIndex, (IImage*)image, glyphOrigin, escapement );
	
}
VG_API_CALL void VG_API_ENTRY vgClearGlyph(VGFont font,VGuint glyphIndex) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgDrawGlyph(VGFont font, 
										  VGuint glyphIndex,
										  VGbitfield paintModes,
										  VGboolean allowAutoHinting) VG_API_EXIT {
	if ( !font ) 
		return;
	
	IFont* f = (IFont*)font;
	f->drawGlyph( glyphIndex, paintModes );
	
}
VG_API_CALL void VG_API_ENTRY vgDrawGlyphs(VGFont font,
										   VGint glyphCount,
										   VGuint *glyphIndices,
										   VGfloat *adjustments_x,
										   VGfloat *adjustments_y,
										   VGbitfield paintModes,
										   VGboolean allowAutoHinting) VG_API_EXIT {
	
}
