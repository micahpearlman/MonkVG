//
//  mkImage.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include "mkImage.h"
#include "mkContext.h"

namespace MonkVG {	// Internal Implementation
	
	IImage::IImage( VGImageFormat format,
		   VGint width, VGint height,
		   VGbitfield allowedQuality )
	:	BaseObject()
	,	_format( format )
	,	_width( width )
	,	_height( height )
	,	_allowedQuality( allowedQuality )
	,	_parent( 0 )
	{
		_s[0] = _t[0] = 0;
		_s[1] = _t[1] = 1;
	}
	
	IImage::IImage( IImage& other ) 
	:	BaseObject()
	,	_format( other._format )
	,	_width( other._width )
	,	_height( other._height )
	,	_allowedQuality( other._allowedQuality )
	,	_parent( &other )
	{
		_s[0] = other._s[0]; 
		_s[1] = other._s[1]; 
		_t[0] = other._t[0]; 
		_t[1] = other._t[1]; 

	}

	VGint IImage::getParameteri( const VGint p ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	VGfloat IImage::getParameterf( const VGint p ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	void IImage::getParameterfv( const VGint p, VGfloat *fv ) const {
		switch (p) {
				
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
		
	}
	
	void IImage::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IImage::setParameter( const VGint p, const VGfloat v ) 
	{
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IImage::setParameter( const VGint p, const VGfloat* fv, const VGint cnt ) {
		switch (p) {
				
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	
	
}


///// OpenVG API Implementation /////

using namespace MonkVG;

VG_API_CALL VGImage VG_API_ENTRY vgCreateImage(VGImageFormat format,
											   VGint width, VGint height,
											   VGbitfield allowedQuality) VG_API_EXIT {
	return (VGImage)IContext::instance().createImage( format, width, height, allowedQuality );
}
VG_API_CALL void VG_API_ENTRY vgDestroyImage(VGImage image) VG_API_EXIT {
	IContext::instance().destroyImage( (IImage*)image );
}
VG_API_CALL void VG_API_ENTRY vgClearImage(VGImage image,
										   VGint x, VGint y, VGint width, VGint height) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgImageSubData(VGImage image,
											 const void * data, VGint dataStride,
											 VGImageFormat dataFormat,
											 VGint x, VGint y, VGint width, VGint height) VG_API_EXIT {
	if ( image == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	IImage* mkImage = (IImage*)image;
	mkImage->setSubData( data, dataStride, dataFormat, x, y, width, height );
	
}
VG_API_CALL void VG_API_ENTRY vgGetImageSubData(VGImage image,
												void * data, VGint dataStride,
												VGImageFormat dataFormat,
												VGint x, VGint y,
											VGint width, VGint height) VG_API_EXIT {
	
}
VG_API_CALL VGImage VG_API_ENTRY vgChildImage(VGImage parent,
											  VGint x, VGint y, VGint width, VGint height) VG_API_EXIT {
	
	if ( parent == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return VG_INVALID_HANDLE;
	}
	
	IImage* p = (IImage*)parent;
	IImage* child = p->createChild( x, y, width, height );

	return (VGImage)child;
	
}
VG_API_CALL VGImage VG_API_ENTRY vgGetParent(VGImage image) VG_API_EXIT; 
VG_API_CALL void VG_API_ENTRY vgCopyImage(VGImage dst, VGint dx, VGint dy,
										  VGImage src, VGint sx, VGint sy,
										  VGint width, VGint height,
										  VGboolean dither) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgDrawImage(VGImage image) VG_API_EXIT {
	if ( image == VG_INVALID_HANDLE ) {
		SetError( VG_BAD_HANDLE_ERROR );
		return;
	}

	// force image matrix mode
	if( IContext::instance().getMatrixMode() != VG_MATRIX_IMAGE_USER_TO_SURFACE ) {
		IContext::instance().setMatrixMode( VG_MATRIX_IMAGE_USER_TO_SURFACE );
	}

	IImage* mkImage = (IImage*)image;
	mkImage->draw();
}
VG_API_CALL void VG_API_ENTRY vgSetPixels(VGint dx, VGint dy,
										  VGImage src, VGint sx, VGint sy,
										  VGint width, VGint height) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgWritePixels(const void * data, VGint dataStride,
											VGImageFormat dataFormat,
											VGint dx, VGint dy,
											VGint width, VGint height) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgGetPixels(VGImage dst, VGint dx, VGint dy,
										  VGint sx, VGint sy,
										  VGint width, VGint height) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgReadPixels(void * data, VGint dataStride,
										   VGImageFormat dataFormat,
										   VGint sx, VGint sy,
										   VGint width, VGint height) VG_API_EXIT {
	
}
VG_API_CALL void VG_API_ENTRY vgCopyPixels(VGint dx, VGint dy,
										   VGint sx, VGint sy,
										   VGint width, VGint height) VG_API_EXIT {
	
}
