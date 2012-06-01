#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#include <MonkVG/openvg.h>
#include <MonkVG/vgu.h>


VGPath testCreatePath();
void testMoveTo(VGPath p, float x, float y, VGPathAbsRel absrel);
void testLineTo(VGPath p, float x, float y, VGPathAbsRel absrel);
void testHlineTo(VGPath p, float x, VGPathAbsRel absrel);
void testVlineTo(VGPath p, float y, VGPathAbsRel absrel);

void testQuadTo(VGPath p, float x1, float y1, float x2, float y2,
                VGPathAbsRel absrel);

void testCubicTo(VGPath p, float x1, float y1, float x2, float y2, float x3, float y3,
                 VGPathAbsRel absrel);

void testSquadTo(VGPath p, float x2, float y2,VGPathAbsRel absrel);

void testScubicTo(VGPath p, float x2, float y2, float x3, float y3,
                  VGPathAbsRel absrel);

void testArcTo(VGPath p, float rx, float ry, float rot, float x, float y,
               VGPathSegment type, VGPathAbsRel absrel);

void testClosePath(VGPath p);

void testOverlayString(const char *format, ...);

void testOverlayColor(float r, float g, float b, float a);


VGint testWidth();
VGint testHeight();

