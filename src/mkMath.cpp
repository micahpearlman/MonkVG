/*
 *  mkMath.cpp
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/11/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "mkMath.h"
#include "mkContext.h"

using namespace MonkVG;

//// OpenVG API Implementation ////

//// Matrix Manipulation ////
VG_API_CALL void VG_API_ENTRY vgLoadIdentity(void) VG_API_EXIT {
	IContext::instance().setIdentity();
}

VG_API_CALL void VG_API_ENTRY vgLoadMatrix(const VGfloat * m) VG_API_EXIT {
	IContext::instance().setTransform( m );
}

VG_API_CALL void VG_API_ENTRY vgGetMatrix(VGfloat * m) VG_API_EXIT {
	IContext::instance().transform( m );
}

VG_API_CALL void VG_API_ENTRY vgMultMatrix(const VGfloat * m) VG_API_EXIT {
	IContext::instance().multiply( m );
}

VG_API_CALL void VG_API_ENTRY vgTranslate(VGfloat tx, VGfloat ty) VG_API_EXIT {
	IContext::instance().translate( tx, ty );
}

VG_API_CALL void VG_API_ENTRY vgScale(VGfloat sx, VGfloat sy) VG_API_EXIT {
	IContext::instance().scale( sx, sy );
}

//VG_API_CALL void VG_API_ENTRY vgShear(VGfloat shx, VGfloat shy) VG_API_EXIT;

VG_API_CALL void VG_API_ENTRY vgRotate(VGfloat angle) VG_API_EXIT {
	IContext::instance().rotate( angle );
}
