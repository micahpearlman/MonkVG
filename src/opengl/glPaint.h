/*
 *  glPaint.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glPaint_h__
#define __glPaint_h__

#include "mkPaint.h"
#include "glPlatform.h"
#include "glImage.h"

namespace MonkVG {

class OpenGLPaint : public IPaint {
  public:
    OpenGLPaint(IContext& context);
    virtual ~OpenGLPaint();    

};
} // namespace MonkVG
#endif // __qzPaint_h__
