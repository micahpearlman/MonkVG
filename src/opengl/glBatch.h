//
//  glBatch.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/27/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __glBatch_h__
#define __glBatch_h__

#include "mkBatch.h"

#include "glPlatform.h"

#include <vector>

namespace MonkVG {
class OpenGLBatch : public IBatch {
  public:
    OpenGLBatch();
    virtual ~OpenGLBatch();

    virtual void draw();
    virtual void dump(void **vertices, size_t *size);
    virtual void finalize();

    void addPathVertexData(GLfloat *fill_verts, size_t fill_vert_cnt,
                           GLfloat *stroke_verts, size_t stroke_vert_cnt,
                           VGbitfield paint_modes);

  public:
    struct vertex_t {
        GLfloat v[2];
        GLuint  color;
    };

  private:
    std::vector<vertex_t> _vertices;
    size_t                _vertexCount;
    GLuint                _vbo;
};
} // namespace MonkVG

#endif // __glBatch_h__
