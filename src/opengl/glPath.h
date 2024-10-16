/*
 *  glPath.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glPath_h__
#define __glPath_h__

#include "mkPath.h"
#include "mkTessellator.h"
#include "glPlatform.h"
#include "glPaint.h"
#include <memory>
#include <vector>

namespace MonkVG {

class OpenGLPath : public IPath {
  public:
    OpenGLPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
               VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint,
               VGbitfield capabilities, IContext &context);
    virtual ~OpenGLPath();

    bool draw(VGbitfield paintModes) override;
    void clear(VGbitfield caps) override;
    void buildFillIfDirty() override;
    void buildStrokeIfDirty() override;

  private:
    struct v2_t {
        GLfloat x, y;
    };


    struct textured_vertex_t {
        GLfloat v[2];
        GLfloat uv[2];
    };

  private:
    std::vector<GLfloat> _fill_vertices = {};
    std::vector<v2_t>    _stroke_verts  = {};

    GLuint _fill_vbo   = GL_UNDEFINED;
    GLuint _fill_vao   = GL_UNDEFINED;
    GLuint _stroke_vbo = GL_UNDEFINED;
    GLuint _stroke_vao = GL_UNDEFINED;

    int          _num_fill_verts   = 0;
    int          _num_stroke_verts = 0;
    OpenGLPaint *_fill_paint       = nullptr;
    OpenGLPaint *_stroke_paint     = nullptr;

    void endOfTesselation(VGbitfield paintModes);

  private: // utility methods

    void buildFill();
    void buildStroke();
    void buildFatLineSegment(std::vector<v2_t> &vertices, const v2_t &p0,
                             const v2_t &p1, const float stroke_width);
};
} // namespace MonkVG

#endif // __glPath_h__
