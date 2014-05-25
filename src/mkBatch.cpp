//
//  mkBatch.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/27/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include "mkBatch.h"
#include "mkContext.h"

namespace MonkVG {	// Internal Implementation
	VGint IBatch::getParameteri( const VGint p ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	VGfloat IBatch::getParameterf( const VGint p ) const {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				return -1;	//todo: set error
				break;
		}
	}
	
	void IBatch::getParameterfv( const VGint p, VGfloat *fv ) const {
		switch (p) {
				
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
		
	}
	
	void IBatch::setParameter( const VGint p, const VGint v ) {
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IBatch::setParameter( const VGint p, const VGfloat v ) 
	{
		switch (p) {
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	void IBatch::setParameter( const VGint p, const VGfloat* fv, const VGint cnt ) {
		switch (p) {
				
			default:
				SetError( VG_ILLEGAL_ARGUMENT_ERROR );
				break;
		}
	}
	
	
	
}


///// OpenVG API Implementation /////

using namespace MonkVG;

VG_API_CALL VGBatchMNK VG_API_ENTRY vgCreateBatchMNK() VG_API_EXIT {
	return (VGBatchMNK)IContext::instance().createBatch();
}

VG_API_CALL void VG_API_ENTRY vgDestroyBatchMNK( VGBatchMNK batch ) VG_API_EXIT {
	IContext::instance().destroyBatch( (IBatch*)batch );
}
VG_API_CALL void VG_API_ENTRY vgBeginBatchMNK( VGBatchMNK batch ) VG_API_EXIT {
	IContext::instance().startBatch( (IBatch*)batch );	
}
VG_API_CALL void VG_API_ENTRY vgEndBatchMNK( VGBatchMNK batch ) VG_API_EXIT {
	IContext::instance().endBatch( (IBatch*)batch );	
}
VG_API_CALL void VG_API_ENTRY vgDrawBatchMNK( VGBatchMNK batch ) VG_API_EXIT {
	((IBatch*)batch)->draw();
}
VG_API_CALL void VG_API_ENTRY vgDumpBatchMNK( VGBatchMNK batch, void **vertices, size_t * size ) VG_API_EXIT {
    IContext::instance().dumpBatch( (IBatch *)batch, vertices, size );
}

