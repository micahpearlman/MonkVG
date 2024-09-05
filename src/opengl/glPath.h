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
#include "glPlatform.h"
#include "glPaint.h"
#include <list>
#include <vector>

namespace MonkVG {

class OpenGLPath : public IPath {

  public:
    OpenGLPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
               VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint,
               VGbitfield capabilities);
    virtual ~OpenGLPath();

    bool draw(VGbitfield paintModes) override;
    void clear(VGbitfield caps) override;
    void buildFillIfDirty() override;

  private:
    struct v2_t {
        GLfloat x, y;
    };

    struct v3_t {
        GLdouble x, y, z;
        v3_t() {}
        v3_t(GLdouble *v) : x(v[0]), y(v[1]), z(v[2]) {}
        v3_t(GLdouble ix, GLdouble iy, GLdouble iz) : x(ix), y(iy), z(iz) {}
        void print() const { printf("(%f, %f)\n", x, y); }
    };

    struct textured_vertex_t {
        GLfloat v[2];
        GLfloat uv[2];
    };

  private:
    GLUtesselator  *_fill_tess        = nullptr;
    vector<GLfloat> _vertices         = {};
    vector<v2_t>    _stroke_verts     = {};
    list<v3_t>      _tess_verts       = {};
    GLenum          _prim_type        = GL_UNDEFINED;
    GLuint          _fill_vbo         = GL_UNDEFINED;
    GLuint          _fill_vao         = GL_UNDEFINED;
    GLuint          _stroke_vbo       = GL_UNDEFINED;
    GLuint          _stroke_vao       = GL_UNDEFINED;
    int             _num_fill_verts   = 0;
    int             _num_stroke_verts = 0;
    OpenGLPaint    *_fill_paint       = nullptr;
    OpenGLPaint    *_stroke_paint     = nullptr;

    /// Glu tesseleator callbacks
    static void tessBegin(GLenum type, GLvoid *user);
    static void tessEnd(GLvoid *user);
    static void tessVertex(GLvoid *vertex, GLvoid *user);
    static void tessCombine(GLdouble coords[3], void *data[4],
                            GLfloat weight[4], void **outData,
                            void *polygonData);
    static void tessError(GLenum errorCode);
    void        endOfTesselation(VGbitfield paintModes);

  private: // utility methods
    GLenum primType() { return _prim_type; }
    void   setPrimType(GLenum t) { _prim_type = t; }

    GLdouble *tessVerticesBackPtr() { return &(_tess_verts.back().x); }

    void updateBounds(float x, float y) {
        _minX   = std::min(_minX, x);
        _width  = std::max(_width, x);
        _minY   = std::min(_minY, y);
        _height = std::max(_height, y);
    }

    void addVertex(GLdouble *v) {
        VGfloat x = (VGfloat)v[0];
        VGfloat y = (VGfloat)v[1];
        updateBounds(x, y);
        _vertices.push_back(x);
        _vertices.push_back(y);
    }

    GLdouble *addTessVertex(const v3_t &v) {
        // updateBounds(v.x, v.y);
        _tess_verts.push_back(v);
        return tessVerticesBackPtr();
    }

    void buildFill();
    void buildStroke();
    void buildFatLineSegment(vector<v2_t> &vertices, const v2_t &p0,
                             const v2_t &p1, const float stroke_width);
};
} // namespace MonkVG

#endif // __glPath_h__
