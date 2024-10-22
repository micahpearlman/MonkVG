/*
 *  glPath.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glPath.h"
#include "glContext.h"
#include "glBatch.h"
#include <cassert>

namespace MonkVG {

OpenGLPath::OpenGLPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
                       VGfloat bias, VGint segmentCapacityHint,
                       VGint coordCapacityHint, VGbitfield capabilities,
                       IContext &context)
    : IPath(pathFormat, datatype, scale, bias, segmentCapacityHint,
            coordCapacityHint, capabilities, context) {}

OpenGLPath::~OpenGLPath() {
    CHECK_GL_ERROR;

    if (_fill_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_fill_vbo);
        _fill_vbo = GL_UNDEFINED;
    }

    if (_fill_vao != GL_UNDEFINED) {
        glDeleteVertexArrays(1, &_fill_vao);
        _fill_vao = GL_UNDEFINED;
    }

    if (_stroke_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_stroke_vbo);
        _stroke_vbo = GL_UNDEFINED;
    }

    if (_stroke_vao != GL_UNDEFINED) {
        glDeleteVertexArrays(1, &_stroke_vao);
        _stroke_vao = GL_UNDEFINED;
    }
    CHECK_GL_ERROR;
}

void OpenGLPath::clear(VGbitfield caps) {
    IPath::clear(caps);

    _fill_vertices.clear();

    // delete vbo buffers
    if (_stroke_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_stroke_vbo);
        _stroke_vbo = GL_UNDEFINED;
    }

    if (_fill_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_fill_vbo);
        _fill_vbo = GL_UNDEFINED;
    }
}

void OpenGLPath::buildFillIfDirty() {
    IPaint *current_fill_paint = getContext().getFillPaint();
    if (current_fill_paint != _fill_paint) {
        _fill_paint = (OpenGLPaint *)current_fill_paint;
        setFillDirty(true);
    }
    // only build the fill if dirty or we are in batch build mode
    if (getIsFillDirty() || getContext().currentBatch()) {
        // tessellate the path
        getContext().getTessellator().tessellate(
            _segments, _fcoords, getContext().getFillRule(),
            getContext().getTessellationIterations(), _fill_vertices, _bounds);
    }
    setFillDirty(false);
}

void OpenGLPath::buildStrokeIfDirty() {
    IPaint *current_stroke_paint = getContext().getStrokePaint();
    if (current_stroke_paint != _stroke_paint) {
        _stroke_paint = (OpenGLPaint *)current_stroke_paint;
        setStrokeDirty(true);
    }
    // only build the fill if dirty or we are in batch build mode
    if (getIsStrokeDirty() || getContext().currentBatch()) {
        getContext().getTessellator().buildStroke(
            _segments, _fcoords, getContext().getStrokeLineWidth(),
            getContext().getTessellationIterations(), _stroke_verts);
    }
    setStrokeDirty(false);
}

bool OpenGLPath::draw(VGbitfield paint_modes) {

    // if there are no paint modes then do nothing
    if (paint_modes == 0) {
        return false;
    }

    CHECK_GL_ERROR;

    // get the native OpenGL context
    OpenGLContext &gl_ctx = (MonkVG::OpenGLContext &)getContext();

    // if dirty build the stroke and fill
    // this will take the path data and build the vertex data
    // through tessellation
    if (paint_modes & VG_FILL_PATH) {
        buildFillIfDirty();
    }

    if (paint_modes & VG_STROKE_PATH) {
        buildStrokeIfDirty();
    }

    buildOpenGLBuffers(paint_modes);

    if (gl_ctx.currentBatch()) {
        return true; // creating a batch so bail from here
    }

    // restore image mode later
    VGImageMode old_image_mode = gl_ctx.getImageMode();

    // configure based on paint type
    if (_fill_paint && _fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
        // set the shader to a color shader
        gl_ctx.bindShader(OpenGLContext::ShaderType::ColorShader);

        // context will setup any uniforms
        getContext().fill();

        // bind the vao & vbo and draw
        glBindVertexArray(_fill_vao);
        glDrawArrays(GL_TRIANGLES, 0, _num_fill_verts);
        glBindVertexArray(0);

    } else if (_fill_paint &&
               (_fill_paint->getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT ||
                _fill_paint->getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT ||
                _fill_paint->getPaintType() ==
                    VG_PAINT_TYPE_RADIAL_2x3_GRADIENT ||
                _fill_paint->getPaintType() ==
                    VG_PAINT_TYPE_LINEAR_2x3_GRADIENT)) {

        // TODO: implement gradient paint. This requires a texture shader
        throw std::runtime_error("not implemented");
        gl_ctx.setImageMode(VG_DRAW_IMAGE_NORMAL);
    }

    // this is important to unbind the vbo when done
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw the stroke last so it renders on top of fill
    if ((paint_modes & VG_STROKE_PATH) && _stroke_vao != GL_UNDEFINED) {
        if (_stroke_paint &&
            _stroke_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
            // set the shader to a color shader
            gl_ctx.bindShader(OpenGLContext::ShaderType::ColorShader);

        } else {
            throw std::runtime_error("Non color stroke paint not implemented");
        }
        // draw
        getContext().stroke();
        glBindVertexArray(_stroke_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _num_stroke_verts);
        glBindVertexArray(0);
    }

    CHECK_GL_ERROR;

    return true;
}

void OpenGLPath::buildOpenGLBuffers(VGbitfield paint_modes) {
    /// build fill vao & vbo
    if (_fill_vertices.size() > 0) {

        // delete the old vbo
        if (_fill_vbo != GL_UNDEFINED) {
            glDeleteBuffers(1, &_fill_vbo);
            _fill_vbo = GL_UNDEFINED;
        }

        // delete the old vao
        if (_fill_vao != GL_UNDEFINED) {
            glDeleteVertexArrays(1, &_fill_vao);
            _fill_vao = GL_UNDEFINED;
        }

        glGenVertexArrays(1, &_fill_vao);
        glGenBuffers(1, &_fill_vbo);
        glBindVertexArray(_fill_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _fill_vbo);
        if (_fill_paint && _fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
            glBufferData(GL_ARRAY_BUFFER,
                         _fill_vertices.size() * sizeof(VGfloat),
                         _fill_vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
            glEnableVertexAttribArray(0);
        } else if (_fill_paint && (_fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_LINEAR_GRADIENT ||
                                   _fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_RADIAL_GRADIENT ||
                                   _fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_RADIAL_2x3_GRADIENT ||
                                   _fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_LINEAR_2x3_GRADIENT)) {
            // throw std::runtime_error("Not implemented");
            std::vector<textured_vertex_2d_t> texturedVertices;
            for (std::vector<float>::const_iterator it = _fill_vertices.begin();
                 it != _fill_vertices.end(); it++) {
                // build up the textured vertex
                textured_vertex_2d_t v;
                v.vert.x = *it;
                it++;
                v.vert.y = *it;
                v.uv[0]  = fabsf(v.vert.x - getMinX()) / getWidth();
                v.uv[1]  = fabsf(v.vert.x - getMinY()) / getHeight();
                texturedVertices.push_back(v);
            }

            glBufferData(GL_ARRAY_BUFFER,
                         texturedVertices.size() * sizeof(textured_vertex_2d_t),
                         &texturedVertices[0], GL_STATIC_DRAW);

            texturedVertices.clear();

            // setup the paints linear gradient
            _fill_paint->buildGradientImage(getWidth(), getHeight());
        }

        _num_fill_verts = (int)_fill_vertices.size() / 2;
    }

    /// build stroke vbo
    if (_stroke_verts.size() > 0) {
        // build the vertex buffer object VBO
        if (_stroke_vbo != GL_UNDEFINED) {
            glDeleteBuffers(1, &_stroke_vbo);
            _stroke_vbo = GL_UNDEFINED;
        }

        if (_stroke_vao != GL_UNDEFINED) {
            glDeleteVertexArrays(1, &_stroke_vao);
            _stroke_vao = GL_UNDEFINED;
        }
        glGenVertexArrays(1, &_stroke_vao);
        glGenBuffers(1, &_stroke_vbo);
        glBindVertexArray(_stroke_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _stroke_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     _stroke_verts.size() * sizeof(vertex_2d_t),
                     _stroke_verts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glEnableVertexAttribArray(0);

        _num_stroke_verts = (int)_stroke_verts.size();
    }

    OpenGLBatch *glBatch = (OpenGLBatch *)getContext().currentBatch();
    if (glBatch) { // if in batch mode update the current batch
        glBatch->addPathVertexData(
            &_fill_vertices[0], _fill_vertices.size() / 2,
            (float *)&_stroke_verts[0], _stroke_verts.size(), paint_modes);
    }

    // clear out vertex buffer
    _fill_vertices.clear();
    _stroke_verts.clear();
}

} // namespace MonkVG
