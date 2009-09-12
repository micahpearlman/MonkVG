/*
 *  qzPath.h
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/3/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __qzPath_h__
#define __qzPath_h__

#include "mkPath.h"

namespace MonkVG {
	
	class QuartzPath : public IPath {
	public:
	
		QuartzPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) 
			:	IPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities ) {
		}
		virtual ~QuartzPath() {
			
		}
		
		
		virtual bool draw( VGbitfield paintModes );
	};
}

#endif // __qzPath_h__