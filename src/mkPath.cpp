/*
 *  mkPath.cpp
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "mkPath.h"
#include "mkContext.h"
#include <cassert>

namespace MonkVG {	// Internal Implementation

	int32_t IPath::segmentToNumCoordinates(VGPathSegment segment)
	{
		static const int32_t coords[13] = {0,2,2,1,1,4,6,2,4,5,5,5,5};
		return coords[(int32_t)segment >> 1];
	}
	
	void IPath::appendData( const VGint numSegments, const VGubyte * pathSegments, const void * pathData ) 
	{
		int numCoords = 0;
		for( int i = 0; i < numSegments; i++ ) {
			_segments.push_back(pathSegments[i] );
			numCoords += segmentToNumCoordinates( static_cast<VGPathSegment>( pathSegments[i] ) );
		}
		
		_numSegments += numSegments;
		_numCoords += numCoords;
		
		for( int i = 0; i < numCoords; i++ ) {
			switch (_datatype) {
				case VG_PATH_DATATYPE_F:
					_fcoords->push_back( *(((VGfloat*)(pathData)) + i) );
					break;
				default:
					// error
					assert( !"unsupported path data type" );
					break;
			}
		}
		
		setIsDirty( true );
	}
	
	void IPath::copy( const IPath& src, const Matrix33& transform ) {
		// TODO: transform!!!
		// BUGBUG
		setNumCoords( src.getNumCoords() );
		setNumSegments( src.getNumSegments() );
		_segments = src._segments;
		*_fcoords = *src._fcoords;
	}
	
	void IPath::clear( VGbitfield caps ) {
	
		_segments.clear();
		_numSegments = 0;
		_numCoords = 0;

		switch (_datatype) {
			case VG_PATH_DATATYPE_F:
				_fcoords->clear();
				break;
			default:
				// error
				assert( !"unsupported path data type" );
				break;
		}
		
	}
	
	VGint IPath::getParameteri( const VGint p ) const {
		switch (p) {
			case VG_PATH_FORMAT:
				return getFormat();
			case VG_PATH_DATATYPE:
				return getDataType();
			case VG_PATH_NUM_SEGMENTS:
				return getNumSegments();
			case VG_PATH_NUM_COORDS:
				return getNumCoords();
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	
				break;
		}
	}
	
	VGfloat IPath::getParameterf( const VGint p ) const {
		switch (p) {
			case VG_PATH_SCALE:
				return getScale();
				break;
			case VG_PATH_BIAS:
				return getBias();
				break;
			default:
				IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	
				break;
		}
	}
	
	void IPath::getParameterfv( const VGint p, VGfloat *fv ) const {
		IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
	}

	
	void IPath::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			case VG_PATH_FORMAT:
				setFormat( v );
				break;
			case VG_PATH_DATATYPE:
				setDataType( static_cast<VGPathDatatype>( v ) );
				break;
			case VG_PATH_NUM_SEGMENTS:
				setNumSegments( v );
				break;
			case VG_PATH_NUM_COORDS:
				setNumCoords( v );
				break;
			default:
				break;
		}
	}
	
	void IPath::setParameter( const VGint p, const VGfloat v ) 
	{
		switch (p) {
			case VG_PATH_SCALE:
				setScale( v );
				break;
			case VG_PATH_BIAS:
				setBias( v );
				break;
			default:
				break;
		}
	}
	
	void IPath::setParameter( const VGint p, const VGfloat* fv, const VGint cnt ) {
		IContext::instance().setError( VG_ILLEGAL_ARGUMENT_ERROR );
	}
	
	
}


///// OpenVG API Implementation /////

using namespace MonkVG;

VG_API_CALL VGPath vgCreatePath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) {
	IPath* path = IContext::instance().createPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities  &= VG_PATH_CAPABILITY_ALL );
	
	return (VGPath)path;
}

VG_API_CALL void VG_API_ENTRY vgDestroyPath(VGPath path) VG_API_EXIT {
	if( path ) {
		IContext::instance().destroyPath( (IPath*)path );
		path = VG_INVALID_HANDLE;
	}
}


VG_API_CALL void vgAppendPathData( VGPath dstPath, VGint numSegments, const VGubyte * pathSegments, const void * pathData ) {
	IPath* path = (IPath*)dstPath;
	path->appendData( numSegments, pathSegments, pathData );
}

VG_API_CALL void vgDrawPath(VGPath path, VGbitfield paintModes) {
	if ( path == VG_INVALID_HANDLE ) {
		IContext::instance().setError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	// force image matrix mode
	if( IContext::instance().getMatrixMode() != VG_MATRIX_PATH_USER_TO_SURFACE ) {
		IContext::instance().setMatrixMode( VG_MATRIX_PATH_USER_TO_SURFACE );
	}

	IPath* p = (IPath*)path;
	
	
	p->draw( paintModes );
}

VG_API_CALL void VG_API_ENTRY vgClearPath(VGPath path, VGbitfield capabilities) VG_API_EXIT {
	if ( path == VG_INVALID_HANDLE ) {
		IContext::instance().setError( VG_BAD_HANDLE_ERROR );
		return;
	}
	
	IPath* p = (IPath*)path;
	p->clear( capabilities );
	
}

VG_API_CALL void VG_API_ENTRY vgTransformPath(VGPath dstPath, VGPath srcPath) VG_API_EXIT {
	IPath* dp = (IPath*)dstPath;
	dp->copy( *(IPath*)srcPath, IContext::instance().getPathUserToSurface() );
}

VG_API_CALL void VG_API_ENTRY vgPathBounds(VGPath path,
										   VGfloat * minX, VGfloat * minY,
										   VGfloat * width, VGfloat * height) VG_API_EXIT {
	
	IPath* p = (IPath*)path;
	p->buildFillIfDirty();	// NOTE: according to the OpenVG specs we only care about the fill bounds, NOT the fill + stroke
	*minX = p->getMinX();
	*minY = p->getMinY();
	*width = p->getWidth();
	*height = p->getHeight();
	
}



