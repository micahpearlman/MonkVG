/*
 *  svg.h
 *  MonkVG-Test-iOS-OpenGL
 *
 *  Created by Micah Pearlman on 8/12/10.
 *  Copyright 2010 Zero Vision. All rights reserved.
 *
 */

#ifndef __SVG_H__
#define __SVG_H__
#include <vg/openvg.h>
#include <vg/vgu.h>
#include <vector>
#include "mkSVG.h"


using namespace std;

class SVGHandler : public MonkSVG::ISVGHandler {
	
public:
	void draw() {
		vgDrawPath( _path, VG_FILL_PATH );
	}
	
private:
	virtual void onPathBegin() { 
		_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,
							 1,0,0,0, VG_PATH_CAPABILITY_ALL);
		
	}
	
	virtual void onPathEnd() {  
		VGubyte seg = VG_CLOSE_PATH;
		VGfloat data = 0.0f;
		vgAppendPathData( _path, 1, &seg, &data );
	}
	
	virtual void onPathMoveTo( float x, float y ) { 
		VGubyte seg = VG_MOVE_TO | VG_ABSOLUTE;
		VGfloat data[2];
		
		data[0] = x; data[1] = y;
		vgAppendPathData( _path, 1, &seg, data );
		
	}
	virtual void onPathLineTo( float x, float y ) { 
		VGubyte seg = VG_LINE_TO | VG_ABSOLUTE;
		VGfloat data[2];
		
		data[0] = x; data[1] = y;
		vgAppendPathData( _path, 1, &seg, data );
		
	}
	virtual void onPathCubic( float x1, float y1, float x2, float y2, float x3, float y3 ) { 
		VGubyte seg = VG_CUBIC_TO | VG_ABSOLUTE;
		VGfloat data[6];
		
		data[0] = x1; data[1] = y1;
		data[2] = x2; data[3] = y2;
		data[4] = x3; data[5] = y3;
		vgAppendPathData( _path, 1, &seg, data);
		
	}
	
	VGPath _path;
};

#endif