/*
 *  glContext.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glContext.h"
#include "glPath.h"
#include "glPaint.h"
#include "glBatch.h"
#include "glImage.h"
#include "glFont.h"
#include "mkCommon.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

/// shaders
#define CPP_GLSL_INCLUDE
#include "shaders/color_vert.glsl"
#include "shaders/color_frag.glsl"
#include "shaders/texture_vert.glsl"
#include "shaders/texture_frag.glsl"

namespace MonkVG {

//// singleton implementation ////
IContext &IContext::instance() {
    static OpenGLContext g_context;
    return g_context;
}

OpenGLContext::OpenGLContext() : IContext() {}

void OpenGLContext::checkGLError() {

    int err = glGetError();

    const char *RVAL = "GL_UNKNOWN_ERROR";

    switch (err) {
    case GL_NO_ERROR:
        RVAL = "GL_NO_ERROR";
        break;
    case GL_INVALID_ENUM:
        RVAL = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        RVAL = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        RVAL = "GL_INVALID_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        RVAL = "GL_OUT_OF_MEMORY";
        break;
    default:
        break;
    }

    if (err != GL_NO_ERROR) {
        MK_LOG("GL_ERROR: %s\n", RVAL);
        MK_ASSERT(0);
    }
}

bool OpenGLContext::Initialize() {

    CHECK_GL_ERROR;

    // create the gl backend context dependent on user selected backend
    if (getRenderingBackendType() == VG_RENDERING_BACKEND_TYPE_OPENGL33) {
        MK_LOG("OpenGL 3.3 backend selected\n");
    } else { // error
        MK_ASSERT(!"ERROR: Unsupported Rendering Backend.");
    }

    // load the shaders
    _color_shader = std::make_unique<OpenGLShader>();
    bool status =
        _color_shader->compile(color_vert.c_str(), color_frag.c_str());
    if (!status) {
        throw std::runtime_error("failed to compile color shader");
        return false;
    }
    _texture_shader = std::make_unique<OpenGLShader>();
    status =
        _texture_shader->compile(texture_vert.c_str(), texture_frag.c_str());
    if (!status) {
        throw std::runtime_error("failed to compile texture shader");
        return false;
    }

    // get viewport to restore back when we are done
    glGetIntegerv(GL_VIEWPORT, _restore_viewport);

    resize();
    CHECK_GL_ERROR;

    glDisable(GL_CULL_FACE);

    // turn on blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR;

    return true;
}

void OpenGLContext::resize() {
    // setup GL projection
    glViewport(0, 0, _width, _height);
    pushOrthoCamera(0, _width, 0, _height, -1, 1);
    CHECK_GL_ERROR;
}

bool OpenGLContext::Terminate() {
    _stroke_paint = nullptr;
    _fill_paint   = nullptr;
    return true;
}

/// factories

IPath *OpenGLContext::createPath(VGint path_format, VGPathDatatype data_type,
                                 VGfloat scale, VGfloat bias,
                                 VGint      segment_capacity_hint,
                                 VGint      coord_capacity_hint,
                                 VGbitfield capabilities) {

    OpenGLPath *path = new OpenGLPath(
        path_format, data_type, scale, bias, segment_capacity_hint,
        coord_capacity_hint, capabilities &= VG_PATH_CAPABILITY_ALL);
    if (path == 0)
        SetError(VG_OUT_OF_MEMORY_ERROR);

    return (IPath *)path;
}

void OpenGLContext::destroyPath(IPath *path) { delete (OpenGLPath *)path; }

void OpenGLContext::destroyPaint(IPaint *paint) { delete (OpenGLPaint *)paint; }

IPaint *OpenGLContext::createPaint() {
    OpenGLPaint *paint = new OpenGLPaint();
    if (paint == 0)
        SetError(VG_OUT_OF_MEMORY_ERROR);
    return (IPaint *)paint;
}

IBatch *OpenGLContext::createBatch() {
    OpenGLBatch *batch = new OpenGLBatch();
    if (batch == 0)
        SetError(VG_OUT_OF_MEMORY_ERROR);
    return (IBatch *)batch;
}

void OpenGLContext::destroyBatch(IBatch *batch) {
    if (batch) {
        delete batch;
    }
}

IImage *OpenGLContext::createImage(VGImageFormat format, VGint width,
                                   VGint height, VGbitfield allowedQuality) {
    return new OpenGLImage(format, width, height, allowedQuality);
}

void OpenGLContext::destroyImage(IImage *image) {
    if (image) {
        delete image;
    }
}

IFont *OpenGLContext::createFont() { return new OpenGLFont(); }
void   OpenGLContext::destroyFont(IFont *font) {
    if (font) {
        delete font;
    }
}

/// state
void OpenGLContext::setStrokePaint(IPaint *paint) {
    if (paint != _stroke_paint) {
        IContext::setStrokePaint(paint);
        OpenGLPaint *glPaint = (OpenGLPaint *)_stroke_paint;
        if (glPaint)
            glPaint->setIsDirty(true);
    }
}

void OpenGLContext::setFillPaint(IPaint *paint) {
    if (paint != _fill_paint) {
        IContext::setFillPaint(paint);
        OpenGLPaint *glPaint = (OpenGLPaint *)_fill_paint;
        if (glPaint)
            glPaint->setIsDirty(true);
    }
}

void OpenGLContext::stroke() {
    CHECK_GL_ERROR;
    if (getStrokePaint() &&
        getStrokePaint()->getPaintType() == VG_PAINT_TYPE_COLOR) {
        const std::array<VGfloat, 4> color = getStrokePaint()->getPaintColor();
        _color_shader->setColor({color[0], color[1], color[2], color[3]});
        CHECK_GL_ERROR;

        getStrokePaint()->setIsDirty(false);
        // set the fill paint to dirty
        if (getFillPaint()) {
            getFillPaint()->setIsDirty(true);
        }
    }
}

void OpenGLContext::fill() {

    if (getFillPaint() == nullptr) {
        return;
    }

    if (_fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
        const std::array<VGfloat, 4> color = getFillPaint()->getPaintColor();
        _color_shader->setColor({color[0], color[1], color[2], color[3]});
        // set the stroke paint to dirty
        if (getStrokePaint()) {
            getStrokePaint()->setIsDirty(true);
        }
    } else {
        throw std::runtime_error("OpenGLContext::fill: unsupported paint type");
    }
}

void OpenGLContext::startBatch(IBatch *batch) {
    assert(_current_batch == 0); // can't have multiple batches going on at once
    _current_batch = batch;
}
void OpenGLContext::dumpBatch(IBatch *batch, void **vertices, size_t *size) {
    _current_batch->dump(vertices, size);
}
void OpenGLContext::endBatch(IBatch *batch) {
    _current_batch->finalize();
    _current_batch = 0;
}

void OpenGLContext::clear(VGint x, VGint y, VGint width, VGint height) {
    // TODO:
}

void OpenGLContext::flush() { glFlush(); }
void OpenGLContext::finish() { glFinish(); }

/**
 * @brief load an OpenVG 3x3 matrix into the current OpenGL 4x4 matrix
 *
 */
void OpenGLContext::setGLActiveMatrix() {
    Matrix33 &active = getActiveMatrix();

    // set identity
    _gl_active_matrix = glm::mat4(1.0f);

    //		a, c, e,			// cos(a) -sin(a) tx
    //		b, d, f,			// sin(a) cos(a)  ty
    //		ff0, ff1, ff2;		// 0      0       1
    _gl_active_matrix[0][0] = active.a;
    _gl_active_matrix[0][1] = active.b;
    _gl_active_matrix[1][0] = active.c;
    _gl_active_matrix[1][1] = active.d;
    _gl_active_matrix[3][0] = active.e;
    _gl_active_matrix[3][1] = active.f;
}

void OpenGLContext::setIdentity() {
    Matrix33 &active = getActiveMatrix();
    active.setIdentity();
    setGLActiveMatrix();
}

void OpenGLContext::transform(VGfloat *t) {
    // a	b	0
    // c	d	0
    // tx	ty	1
    Matrix33 &active = getActiveMatrix();
    for (int i = 0; i < 9; i++)
        t[i] = active.m[i];
}

void OpenGLContext::setTransform(const VGfloat *t) {
    //	OpenVG:
    //	sh	shx	tx
    //	shy	sy	ty
    //	0	0	1
    //
    // OpenGL
    // a	b	0
    // c	d	0
    // tx	ty	1

    Matrix33 &active = getActiveMatrix();
    for (int i = 0; i < 9; i++)
        active.m[i] = t[i];
    setGLActiveMatrix();
}

void OpenGLContext::multiply(const VGfloat *t) {
    Matrix33 m;
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            m.set(y, x, t[(y * 3) + x]);
        }
    }
    Matrix33 &active = getActiveMatrix();
    active.postMultiply(m);
    setGLActiveMatrix();
}

void OpenGLContext::scale(VGfloat sx, VGfloat sy) {
    Matrix33 &active = getActiveMatrix();
    Matrix33  scale;
    scale.setIdentity();
    scale.setScale(sx, sy);
    Matrix33 tmp;
    Matrix33::multiply(tmp, scale, active);
    active.copy(tmp);
    setGLActiveMatrix();
}
void OpenGLContext::translate(VGfloat x, VGfloat y) {

    Matrix33 &active = getActiveMatrix();
    Matrix33  translate;
    translate.setTranslate(x, y);
    Matrix33 tmp;
    tmp.setIdentity();
    Matrix33::multiply(tmp, translate, active);
    active.copy(tmp);
    setGLActiveMatrix();
}
void OpenGLContext::rotate(VGfloat angle) {
    Matrix33 &active = getActiveMatrix();
    Matrix33  rotate;
    rotate.setRotation(radians(angle));
    Matrix33 tmp;
    tmp.setIdentity();
    Matrix33::multiply(tmp, rotate, active);
    active.copy(tmp);
    setGLActiveMatrix();
}

void OpenGLContext::setImageMode(VGImageMode im) {
    IContext::setImageMode(im);
    switch (im) {
    case VG_DRAW_IMAGE_NORMAL:
        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        break;
    case VG_DRAW_IMAGE_MULTIPLY:
        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        break;
    case VG_DRAW_IMAGE_STENCIL:
        break;
    default:
        break;
    }
}

void OpenGLContext::pushOrthoCamera(VGfloat left, VGfloat right, VGfloat bottom,
                                    VGfloat top, VGfloat near, VGfloat far) {
    glm::mat4 projection = glm::ortho(left, right, bottom, top, near, far);
    _projection_stack.push(projection);
}

void OpenGLContext::popOrthoCamera() {
    if (_projection_stack.size() > 0) {
        _projection_stack.pop();
    }
}

const glm::mat4 &OpenGLContext::getGLActiveMatrix() {
    return _gl_active_matrix;
}

const glm::mat4 &OpenGLContext::getGLProjectionMatrix() {
    return _projection_stack.top();
}

void OpenGLContext::bindShader(ShaderType shader) {
    if (_current_shader != shader) {
        switch (shader) {
        case ColorShader:
            _color_shader->bind();
            break;
        case TextureShader:
            _texture_shader->bind();
            break;
        case GradientShader:
            _gradient_shader->bind();
            break;
        case None:
            glUseProgram(0);
            break;
        default:
            throw std::runtime_error(
                "OpenGLContext::useShader: invalid shader type");
        }
        _current_shader = shader;
    }
    if (_current_shader != None) {
        // set the shader projection and modelview matrices
        getCurrentShader().setProjectionMatrix(getGLProjectionMatrix());
        getCurrentShader().setModelViewMatrix(getGLActiveMatrix());
    }
}

OpenGLShader &OpenGLContext::getCurrentShader() {
    switch (_current_shader) {
    case ColorShader:
        return *_color_shader;
    case TextureShader:
        return *_texture_shader;
    case GradientShader:
        return *_gradient_shader;
    default:
        throw std::runtime_error(
            "OpenGLContext::getCurrentShader: invalid shader type");
    }
}

} // namespace MonkVG
