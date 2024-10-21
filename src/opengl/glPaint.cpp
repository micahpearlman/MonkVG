/*
 *  glPaint.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glPaint.h"
#include <cmath>
#include <assert.h>
#include "glContext.h"


namespace MonkVG {

OpenGLPaint::OpenGLPaint(IContext &context)
    : IPaint(context) {}

OpenGLPaint::~OpenGLPaint() {
}



} // namespace MonkVG
