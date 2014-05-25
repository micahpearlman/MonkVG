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
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	VGfloat IFont::getParameterf( const VGint p ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	void IFont::getParameterfv( const VGint p, VGfloat *fv ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
		
	}
	
	void IFont::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IFont::setParameter( const VGint p, const VGfloat v ) {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IFont::setParameter( const VGint p, const VGfloat* fv, const VGint cnt ) {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	
	void IFont::addGlyphImage( VGuint index_, IImage* image_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] ) {
		_glyphs[index_] = std::shared_ptr<IFont::GlyphImage>( new IFont::GlyphImage( index_, image_, glyphOrigin_, escapement_ ) );
	}
	void IFont::addGlyphPath( VGuint index_, IPath* path_, VGfloat glyphOrigin_[2], VGfloat escapement_[2] ) {
		_glyphs[index_] = std::shared_ptr<IFont::GlyphPath>( new IFont::GlyphPath( index_, path_, glyphOrigin_, escapement_ ));
	}
	
	void IFont::drawGlyph( VGuint index, VGfloat adj_x, VGfloat adj_y, VGbitfield paintModes ) {
		std::unordered_map<VGuint, std::shared_ptr<Glyph> >::iterator it =  _glyphs.find( index );
		if ( it != _glyphs.end() ) {
			std::shared_ptr<Glyph> glyph = it->second;
			VGfloat origin[2];
			IContext::instance().getGlyphOrigin( origin );
//			vgTranslate( origin[0], origin[1] );
			glyph->draw( paintModes, adj_x, adj_y );
			origin[0] += glyph->escapement[0];
			IContext::instance().setGlyphOrigin( origin );
		}
	}
	
	void IFont::GlyphImage::draw( VGbitfield paintModes, VGfloat adj_x, VGfloat adj_y ) {
		//image->drawSubRect( glyphOrigin[0], glyphOrigin[1], escapement[0], escapement[1], paintModes );
		//image->draw( );
		VGfloat origin[2];
		IContext::instance().getGlyphOrigin( origin );
		origin[0] += adj_x;
		origin[1] += adj_y;
		
		image->drawAtPoint( origin[0], origin[1], paintModes );
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
	if ( font == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	IFont* f = (IFont*)font;
	f->addGlyphPath( glyphIndex, (IPath*)path, glyphOrigin, escapement );
	
}
VG_API_CALL void VG_API_ENTRY vgSetGlyphToImage(VGFont font,
												VGuint glyphIndex,
												VGImage image,
												VGfloat glyphOrigin [2],
												VGfloat escapement[2]) VG_API_EXIT {
	if ( font == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	IFont* f = (IFont*)font;
	f->addGlyphImage( glyphIndex, (IImage*)image, glyphOrigin, escapement );
	
}
VG_API_CALL void VG_API_ENTRY vgClearGlyph(VGFont font,VGuint glyphIndex) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgDrawGlyph(VGFont font, 
										  VGuint glyphIndex,
										  VGbitfield paintModes,
										  VGboolean allowAutoHinting) VG_API_EXIT {
	if ( font == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	// force glyph matrix mode
	if( IContext::instance().getMatrixMode() != VG_MATRIX_GLYPH_USER_TO_SURFACE ) {
		IContext::instance().setMatrixMode( VG_MATRIX_GLYPH_USER_TO_SURFACE );
	}
	IFont* f = (IFont*)font;
	f->drawGlyph( glyphIndex, 0,0, paintModes );
	
	
}
VG_API_CALL void VG_API_ENTRY vgDrawGlyphs(VGFont font,
										   VGint glyphCount,
										   VGuint *glyphIndices,
										   VGfloat *adjustments_x,
										   VGfloat *adjustments_y,
										   VGbitfield paintModes,
										   VGboolean allowAutoHinting) VG_API_EXIT {
	
	if ( font == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	IFont* f = (IFont*)font;

	// force glyph matrix mode
	if( IContext::instance().getMatrixMode() != VG_MATRIX_GLYPH_USER_TO_SURFACE ) {
		IContext::instance().setMatrixMode( VG_MATRIX_GLYPH_USER_TO_SURFACE );
	}
	
	for( int i = 0; i < glyphCount; i++ ) {
		VGfloat ax = 0, ay = 0;
		if ( adjustments_x ) {
			ax = adjustments_x[i];
		}
		if ( adjustments_y ) {
			ay = adjustments_y[i];
		}

		f->drawGlyph( glyphIndices[i], ax, ay, paintModes );
	}
	
}
