#include "test.h"

static int testW = 320;
static int testH = 480;

static float overcolor[4] = {0,0,0,1};


VGPath testCreatePath()
{
  return vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,
                      1,0,0,0, VG_PATH_CAPABILITY_ALL);
}

void testMoveTo(VGPath p, float x, float y, VGPathAbsRel absrel)
{
  VGubyte seg = VG_MOVE_TO | absrel;
  VGfloat data[2];
  
  data[0] = x; data[1] = y;
  vgAppendPathData(p, 1, &seg, data);
}

void testLineTo(VGPath p, float x, float y, VGPathAbsRel absrel)
{
  VGubyte seg = VG_LINE_TO | absrel;
  VGfloat data[2];
  
  data[0] = x; data[1] = y;
  vgAppendPathData(p, 1, &seg, data);
}

void testHlineTo(VGPath p, float x, VGPathAbsRel absrel)
{
  VGubyte seg = VG_HLINE_TO | absrel;
  VGfloat data = x;
  
  vgAppendPathData(p, 1, &seg, &data);
}

void testVlineTo(VGPath p, float y, VGPathAbsRel absrel)
{
  VGubyte seg = VG_VLINE_TO | absrel;
  VGfloat data = y;
  
  vgAppendPathData(p, 1, &seg, &data);
}

void testQuadTo(VGPath p, float x1, float y1, float x2, float y2,
                VGPathAbsRel absrel)
{
  VGubyte seg = VG_QUAD_TO | absrel;
  VGfloat data[4];
  
  data[0] = x1; data[1] = y1;
  data[2] = x2; data[3] = y2;
  vgAppendPathData(p, 1, &seg, data);
}

void testCubicTo(VGPath p, float x1, float y1, float x2, float y2, float x3, float y3,
                 VGPathAbsRel absrel)
{
  VGubyte seg = VG_CUBIC_TO | absrel;
  VGfloat data[6];
  
  data[0] = x1; data[1] = y1;
  data[2] = x2; data[3] = y2;
  data[4] = x3; data[5] = y3;
  vgAppendPathData(p, 1, &seg, data);
}

void testSquadTo(VGPath p, float x2, float y2,VGPathAbsRel absrel)
{
  VGubyte seg = VG_SQUAD_TO | absrel;
  VGfloat data[2];
  
  data[0] = x2; data[1] = y2;
  vgAppendPathData(p, 1, &seg, data);
}

void testScubicTo(VGPath p, float x2, float y2, float x3, float y3,
                  VGPathAbsRel absrel)
{
  VGubyte seg = VG_SCUBIC_TO | absrel;
  VGfloat data[4];
  
  data[0] = x2; data[1] = y2;
  data[2] = x3; data[3] = y3;
  vgAppendPathData(p, 1, &seg, data);
}

void testArcTo(VGPath p, float rx, float ry, float rot, float x, float y,
               VGPathSegment type, VGPathAbsRel absrel)
{
  VGubyte seg = type | absrel;
  VGfloat data[5];
  
  data[0] = rx; data[1] = ry;
  data[2] = rot;
  data[3] = x;  data[4] = y;
  vgAppendPathData(p, 1, &seg, data);
}

void testClosePath(VGPath p)
{
  VGubyte seg = VG_CLOSE_PATH;
  VGfloat data = 0.0f;
  vgAppendPathData(p, 1, &seg, &data);
}

void testOverlayColor(float r, float g, float b, float a)
{
  overcolor[0] = r;
  overcolor[1] = g;
  overcolor[2] = b;
  overcolor[3] = a;
}

VGint testWidth()
{
  return testW;
}

VGint testHeight()
{
  return testH;
}

