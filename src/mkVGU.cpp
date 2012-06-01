/*
 *  mkVGU.cpp
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */


/*------------------------------------------------------------------------
 *
 * VGU library for OpenVG 1.1 Reference Implementation
 * ---------------------------------------------------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Materials. 
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//**
  * \file
  * \brief	Implementation of the VGU utility library for OpenVG
  *//*-------------------------------------------------------------------*/

#include "MonkVG/vgu.h"
#include "MonkVG/openvg.h"
#include "mkCommon.h"
#include "mkMath.h"

//#include "riMath.h"

//using namespace OpenVGRI;

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

static void append(VGPath path, int numSegments, const VGubyte* segments, int numCoordinates, const VGfloat* coordinates)
{
	MK_ASSERT(numCoordinates <= 26);
	
	VGPathDatatype datatype = (VGPathDatatype)vgGetParameteri(path, VG_PATH_DATATYPE);
	VGfloat scale = vgGetParameterf(path, VG_PATH_SCALE);
	VGfloat bias = vgGetParameterf(path, VG_PATH_BIAS);
	
	switch(datatype)
	{
		case VG_PATH_DATATYPE_S_8:
		{
			
			int8_t data[26];
			for(int i=0;i<numCoordinates;i++)
				data[i] = (int8_t)floor((coordinates[i] - bias) / scale + 0.5f);	//add 0.5 for correct rounding
			vgAppendPathData(path, numSegments, segments, data);
			break;
		}
			
		case VG_PATH_DATATYPE_S_16:
		{
			
			int16_t data[26];
			for(int i=0;i<numCoordinates;i++)
				data[i] = (int16_t)floor((coordinates[i] - bias) / scale + 0.5f);	//add 0.5 for correct rounding
			vgAppendPathData(path, numSegments, segments, data);
			break;
		}
			
		case VG_PATH_DATATYPE_S_32:
		{
			int32_t data[26];
			for(int i=0;i<numCoordinates;i++)
				data[i] = (int32_t)floor((coordinates[i] - bias) / scale + 0.5f);	//add 0.5 for correct rounding
			vgAppendPathData(path, numSegments, segments, data);
			break;
		}
			
		default:
		{
			MK_ASSERT(datatype == VG_PATH_DATATYPE_F);
			float data[26];
			for(int i=0;i<numCoordinates;i++)
				data[i] = (float)((coordinates[i] - bias) / scale);
			vgAppendPathData(path, numSegments, segments, data);
			break;
		}
	}
}

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguLine(VGPath path, VGfloat x0, VGfloat y0, VGfloat x1, VGfloat y1)
{
	VGErrorCode error = vgGetError();	//clear the error state
	static const VGubyte segments[2] = {VG_MOVE_TO | VG_ABSOLUTE, VG_LINE_TO | VG_ABSOLUTE};
	const VGfloat data[4] = {x0, y0, x1, y1};
	append(path, 2, segments, 4, data);
	
	error = vgGetError();
	if(error == VG_BAD_HANDLE_ERROR)
		return VGU_BAD_HANDLE_ERROR;
	else if(error == VG_PATH_CAPABILITY_ERROR)
		return VGU_PATH_CAPABILITY_ERROR;
	return VGU_NO_ERROR;
}

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguPolygon(VGPath path, const VGfloat * points, VGint count, VGboolean closed)
{
	
	VGErrorCode error = vgGetError();	//clear the error state
	if(!points || (((ptrdiff_t)points) & 3) || count <= 0)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	VGubyte segments[1] = {VG_MOVE_TO | VG_ABSOLUTE};
	VGfloat data[2];
	for(int i=0;i<count;i++)
	{
		data[0] = points[i*2+0];
		data[1] = points[i*2+1];
		append(path, 1, segments, 2, data);
		segments[0] = VG_LINE_TO | VG_ABSOLUTE;
	}
	if(closed)
	{
		segments[0] = VG_CLOSE_PATH;
		append(path, 1, segments, 0, data);
	}
	
	error = vgGetError();
	if(error == VG_BAD_HANDLE_ERROR)
		return VGU_BAD_HANDLE_ERROR;
	else if(error == VG_PATH_CAPABILITY_ERROR)
		return VGU_PATH_CAPABILITY_ERROR;
	return VGU_NO_ERROR;
}

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height)
{
	VGErrorCode error = vgGetError();	//clear the error state
	if(width <= 0 || height <= 0)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	static const VGubyte segments[5] = {VG_MOVE_TO | VG_ABSOLUTE,
		VG_HLINE_TO | VG_ABSOLUTE,
		VG_VLINE_TO | VG_ABSOLUTE,
		VG_HLINE_TO | VG_ABSOLUTE,
	VG_CLOSE_PATH};
	const VGfloat data[5] = {x, y, x + width, y + height, x};
	append(path, 5, segments, 5, data);
	
	error = vgGetError();
	if(error == VG_BAD_HANDLE_ERROR)
		return VGU_BAD_HANDLE_ERROR;
	else if(error == VG_PATH_CAPABILITY_ERROR)
		return VGU_PATH_CAPABILITY_ERROR;
	return VGU_NO_ERROR;
}



VGUErrorCode vguEllipse(VGPath path, VGfloat cx, VGfloat cy, VGfloat width, VGfloat height)
{
	VGErrorCode error = vgGetError();	//clear the error state
	if(width <= 0 || height <= 0)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	static const VGubyte segments[4] = {VG_MOVE_TO | VG_ABSOLUTE,
		VG_SCCWARC_TO | VG_ABSOLUTE,
		VG_SCCWARC_TO | VG_ABSOLUTE,
		VG_CLOSE_PATH};
	const VGfloat data[12] = {cx + width/2, cy,
		width/2, height/2, 0, cx - width/2, cy,
		width/2, height/2, 0, cx + width/2, cy};
	append(path, 4, segments, 12, data);
	
	error = vgGetError();
	if(error == VG_BAD_HANDLE_ERROR)
		return VGU_BAD_HANDLE_ERROR;
	else if(error == VG_PATH_CAPABILITY_ERROR)
		return VGU_PATH_CAPABILITY_ERROR;
	return VGU_NO_ERROR;
}

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguRoundRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height, VGfloat arcWidth, VGfloat arcHeight)
{
	VGErrorCode error = vgGetError();	//clear the error state
	if(width <= 0 || height <= 0)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	arcWidth = RI_CLAMP(arcWidth, 0.0f, width);
	arcHeight = RI_CLAMP(arcHeight, 0.0f, height);
	
	static const VGubyte segments[10] = {VG_MOVE_TO | VG_ABSOLUTE,
		VG_HLINE_TO | VG_ABSOLUTE,
		VG_SCCWARC_TO | VG_ABSOLUTE,
		VG_VLINE_TO | VG_ABSOLUTE,
		VG_SCCWARC_TO | VG_ABSOLUTE,
		VG_HLINE_TO | VG_ABSOLUTE,
		VG_SCCWARC_TO | VG_ABSOLUTE,
		VG_VLINE_TO | VG_ABSOLUTE,
		VG_SCCWARC_TO | VG_ABSOLUTE,
	VG_CLOSE_PATH};
	const VGfloat data[26] = {x + arcWidth/2, y,
		x + width - arcWidth/2,
		arcWidth/2, arcHeight/2, 0, x + width, y + arcHeight/2,
		y + height - arcHeight/2,
		arcWidth/2, arcHeight/2, 0, x + width - arcWidth/2, y + height,
		x + arcWidth/2,
		arcWidth/2, arcHeight/2, 0, x, y + height - arcHeight/2,
		y + arcHeight/2,
	arcWidth/2, arcHeight/2, 0, x + arcWidth/2, y};
	append(path, 10, segments, 26, data);
	
	error = vgGetError();
	if(error == VG_BAD_HANDLE_ERROR)
		return VGU_BAD_HANDLE_ERROR;
	else if(error == VG_PATH_CAPABILITY_ERROR)
		return VGU_PATH_CAPABILITY_ERROR;
	return VGU_NO_ERROR;
}


/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguArc(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height, VGfloat startAngle, VGfloat angleExtent, VGUArcType arcType)
{
	VGErrorCode error = vgGetError();	//clear the error state
	if((arcType != VGU_ARC_OPEN && arcType != VGU_ARC_CHORD && arcType != VGU_ARC_PIE) || width <= 0.0f || height <= 0.0f)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	startAngle = MonkVG::radians(startAngle);
	angleExtent = MonkVG::radians(angleExtent);
	
	VGfloat w = width/2.0f;
	VGfloat h = height/2.0f;
	
	VGubyte segments[1];
	VGfloat data[5];
	
	segments[0] = VG_MOVE_TO | VG_ABSOLUTE;
	data[0] = x + w * (VGfloat)cos(startAngle);
	data[1] = y + h * (VGfloat)sin(startAngle);
	append(path, 1, segments, 2, data);
	
	data[0] = w;
	data[1] = h;
	data[2] = 0.0f;
	VGfloat endAngle = startAngle + angleExtent;
	if(angleExtent >= 0.0f)
	{
		segments[0] = VG_SCCWARC_TO | VG_ABSOLUTE;
		for(VGfloat a = startAngle + M_PI;a < endAngle; a += M_PI)
		{
			data[3] = x + w * (VGfloat)cos(a);
			data[4] = y + h * (VGfloat)sin(a);
			append(path, 1, segments, 5, data);
		}
	}
	else
	{
		segments[0] = VG_SCWARC_TO | VG_ABSOLUTE;
		for(VGfloat a = startAngle - M_PI;a > endAngle; a -= M_PI)
		{
			data[3] = x + w * (VGfloat)cos(a);
			data[4] = y + h * (VGfloat)sin(a);
			append(path, 1, segments, 5, data);
		}
	}
	data[3] = x + w * (VGfloat)cos(endAngle);
	data[4] = y + h * (VGfloat)sin(endAngle);
	append(path, 1, segments, 5, data);
	
	if(arcType == VGU_ARC_CHORD)
	{
		segments[0] = VG_CLOSE_PATH;
		append(path, 1, segments, 0, data);
	}
	else if(arcType == VGU_ARC_PIE)
	{
		segments[0] = VG_LINE_TO | VG_ABSOLUTE;
		data[0] = x;
		data[1] = y;
		append(path, 1, segments, 2, data);
		segments[0] = VG_CLOSE_PATH;
		append(path, 1, segments, 0, data);
	}
	
	error = vgGetError();
	if(error == VG_BAD_HANDLE_ERROR)
		return VGU_BAD_HANDLE_ERROR;
	else if(error == VG_PATH_CAPABILITY_ERROR)
		return VGU_PATH_CAPABILITY_ERROR;
	return VGU_NO_ERROR;
}

#if 0 // todo

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguComputeWarpQuadToSquare(VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1, VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3, VGfloat * matrix)
{
	if(!matrix || ((ptrdiff_t)matrix) & 3)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	VGfloat mat[9];
	VGUErrorCode ret = vguComputeWarpSquareToQuad(sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, mat);
	if(ret == VGU_BAD_WARP_ERROR)
		return VGU_BAD_WARP_ERROR;
	Matrix3x3 m(mat[0], mat[3], mat[6],
				mat[1], mat[4], mat[7],
				mat[2], mat[5], mat[8]);
	bool nonsingular = m.invert();
	if(!nonsingular)
		return VGU_BAD_WARP_ERROR;
	matrix[0] = m[0][0];
	matrix[1] = m[1][0];
	matrix[2] = m[2][0];
	matrix[3] = m[0][1];
	matrix[4] = m[1][1];
	matrix[5] = m[2][1];
	matrix[6] = m[0][2];
	matrix[7] = m[1][2];
	matrix[8] = m[2][2];
	return VGU_NO_ERROR;
}

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguComputeWarpSquareToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1, VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3, VGfloat * matrix)
{
	if(!matrix || ((ptrdiff_t)matrix) & 3)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	//from Heckbert:Fundamentals of Texture Mapping and Image Warping
	//Note that his mapping of vertices is different from OpenVG's
	//(0,0) => (dx0,dy0)
	//(1,0) => (dx1,dy1)
	//(0,1) => (dx2,dy2)
	//(1,1) => (dx3,dy3)
	
	VGfloat diffx1 = dx1 - dx3;
	VGfloat diffy1 = dy1 - dy3;
	VGfloat diffx2 = dx2 - dx3;
	VGfloat diffy2 = dy2 - dy3;
	
	VGfloat det = diffx1*diffy2 - diffx2*diffy1;
	if(det == 0.0f)
		return VGU_BAD_WARP_ERROR;
	
	VGfloat sumx = dx0 - dx1 + dx3 - dx2;
	VGfloat sumy = dy0 - dy1 + dy3 - dy2;
	
	if(sumx == 0.0f && sumy == 0.0f)
	{	//affine mapping
		matrix[0] = dx1 - dx0;
		matrix[1] = dy1 - dy0;
		matrix[2] = 0.0f;
		matrix[3] = dx3 - dx1;
		matrix[4] = dy3 - dy1;
		matrix[5] = 0.0f;
		matrix[6] = dx0;
		matrix[7] = dy0;
		matrix[8] = 1.0f;
		return VGU_NO_ERROR;
	}
	
	VGfloat oodet = 1.0f / det;
	VGfloat g = (sumx*diffy2 - diffx2*sumy) * oodet;
	VGfloat h = (diffx1*sumy - sumx*diffy1) * oodet;
	
	matrix[0] = dx1-dx0+g*dx1;
	matrix[1] = dy1-dy0+g*dy1;
	matrix[2] = g;
	matrix[3] = dx2-dx0+h*dx2;
	matrix[4] = dy2-dy0+h*dy2;
	matrix[5] = h;
	matrix[6] = dx0;
	matrix[7] = dy0;
	matrix[8] = 1.0f;
	return VGU_NO_ERROR;
}

/*-------------------------------------------------------------------*//*!
 * \brief	
 * \param	
 * \return	
 * \note		
 *//*-------------------------------------------------------------------*/

VGUErrorCode vguComputeWarpQuadToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1, VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3, VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1, VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3, VGfloat * matrix)
{
	if(!matrix || ((ptrdiff_t)matrix) & 3)
		return VGU_ILLEGAL_ARGUMENT_ERROR;
	
	VGfloat qtos[9];
	VGUErrorCode ret1 = vguComputeWarpQuadToSquare(sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, qtos);
	if(ret1 == VGU_BAD_WARP_ERROR)
		return VGU_BAD_WARP_ERROR;
	
	VGfloat stoq[9];
	VGUErrorCode ret2 = vguComputeWarpSquareToQuad(dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3, stoq);
	if(ret2 == VGU_BAD_WARP_ERROR)
		return VGU_BAD_WARP_ERROR;
	
	Matrix3x3 m1(qtos[0], qtos[3], qtos[6],
				 qtos[1], qtos[4], qtos[7],
				 qtos[2], qtos[5], qtos[8]);
	Matrix3x3 m2(stoq[0], stoq[3], stoq[6],
				 stoq[1], stoq[4], stoq[7],
				 stoq[2], stoq[5], stoq[8]);
	Matrix3x3 r = m2 * m1;
	
	matrix[0] = r[0][0];
	matrix[1] = r[1][0];
	matrix[2] = r[2][0];
	matrix[3] = r[0][1];
	matrix[4] = r[1][1];
	matrix[5] = r[2][1];
	matrix[6] = r[0][2];
	matrix[7] = r[1][2];
	matrix[8] = r[2][2];
	return VGU_NO_ERROR;
}
#endif // 0 todo