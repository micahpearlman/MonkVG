//
//  glBatch.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/27/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include <cstring> // for std::memcpy
#include "glBatch.h"
#include "glContext.h"

namespace MonkVG {

OpenGLBatch::OpenGLBatch() : IBatch(), _vbo(-1), _vertexCount(0) {}
OpenGLBatch::~OpenGLBatch() {
    if (_vbo != -1) {
        glDeleteBuffers(1, &_vbo);
        _vbo = -1;
    }
}

void OpenGLBatch::addPathVertexData(GLfloat *fill_verts, size_t fill_vert_cnt,
                                    GLfloat   *stroke_verts,
                                    size_t     stroke_vert_cnt,
                                    VGbitfield paint_modes) {

    // get the current transform
    Matrix33 &transform = IContext::instance().getActiveMatrix();

    if (paint_modes & VG_FILL_PATH) {
        vertex_t vert;

        // get the paint color
        IPaint        *paint = IContext::instance().getFillPaint();
        const VGfloat *fc    = paint->getPaintColor();

        vert.color = (uint32_t(fc[3] * 255.0f) << 24)   // a
                     | (uint32_t(fc[2] * 255.0f) << 16) // b
                     | (uint32_t(fc[1] * 255.0f) << 8)  // g
                     | (uint32_t(fc[0] * 255.0f) << 0); // r

        // get vertices and transform them
        VGfloat v[2];
        for (int i = 0; i < fill_vert_cnt * 2; i += 2) {
            v[0] = fill_verts[i];
            v[1] = fill_verts[i + 1];
            affineTransform(vert.v, transform, v);
            _vertices.push_back(vert);
        }
    }

    if (paint_modes & VG_STROKE_PATH) {
        vertex_t vert, start_vert, last_vert;

        // get the paint color
        IPaint        *paint = IContext::instance().getStrokePaint();
        const VGfloat *fc    = paint->getPaintColor();

        vert.color = (uint32_t(fc[3] * 255.0f) << 24)   // a
                     | (uint32_t(fc[2] * 255.0f) << 16) // b
                     | (uint32_t(fc[1] * 255.0f) << 8)  // g
                     | (uint32_t(fc[0] * 255.0f) << 0); // r

        // get vertices and transform them
        VGfloat v[2];
        int     vertcnt = 0;
        for (int i = 0; i < stroke_vert_cnt * 2; i += 2, vertcnt++) {
            v[0] = stroke_verts[i];
            v[1] = stroke_verts[i + 1];
            affineTransform(vert.v, transform, v);

            // for stroke we need to convert from a strip to triangle
            switch (vertcnt) {
            case 0:
                _vertices.push_back(vert);
                break;
            case 1:
                start_vert = vert;
                _vertices.push_back(vert);
                break;
            case 2:
                last_vert = vert;
                _vertices.push_back(vert);
                break;

            default:
                _vertices.push_back(start_vert);
                _vertices.push_back(last_vert);
                _vertices.push_back(vert);
                start_vert = last_vert;
                last_vert  = vert;
                break;
            }
        }
    }
}

void OpenGLBatch::finalize() {
    // build the vbo
    if (_vbo != -1) {
        glDeleteBuffers(1, &_vbo);
        _vbo = -1;
    }

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(vertex_t),
                 &_vertices[0], GL_STATIC_DRAW);
    _vertexCount = _vertices.size();
    _vertices.clear();
}

void OpenGLBatch::dump(void **vertices, size_t *size) {

    *size     = _vertices.size() * sizeof(vertex_t);
    *vertices = malloc(*size);

    std::memcpy(*vertices, &_vertices[0], *size);
}

void OpenGLBatch::draw() {
    // get the native OpenGL context
    OpenGLContext &glContext = (MonkVG::OpenGLContext &)IContext::instance();

    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // draw
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexPointer(2, GL_FLOAT, sizeof(vertex_t),
                    (GLvoid *)offsetof(vertex_t, v));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vertex_t),
                   (GLvoid *)offsetof(vertex_t, color));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)_vertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace MonkVG
