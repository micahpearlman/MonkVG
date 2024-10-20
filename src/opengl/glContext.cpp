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
        coord_capacity_hint, capabilities &= VG_PATH_CAPABILITY_ALL, *this);
    if (path == 0) {
        SetError(VG_OUT_OF_MEMORY_ERROR);
    }

    return (IPath *)path;
}

void OpenGLContext::destroyPath(IPath *path) { path->decRef(); }

void OpenGLContext::destroyPaint(IPaint *paint) { paint->decRef(); }

IPaint *OpenGLContext::createPaint() {
    OpenGLPaint *paint = new OpenGLPaint(*this);
    if (paint == 0)
        SetError(VG_OUT_OF_MEMORY_ERROR);
    return (IPaint *)paint;
}

IBatch *OpenGLContext::createBatch() {
    OpenGLBatch *batch = new OpenGLBatch(*this);
    if (batch == 0)
        SetError(VG_OUT_OF_MEMORY_ERROR);
    return (IBatch *)batch;
}

void OpenGLContext::destroyBatch(IBatch *batch) {
    if (batch) {
        batch->decRef();
    }
}

IImage *OpenGLContext::createImage(VGImageFormat format, VGint width,
                                   VGint height, VGbitfield allowedQuality) {
    return new OpenGLImage(format, width, height, allowedQuality, *this);
}

void OpenGLContext::destroyImage(IImage *image) {
    if (image) {
        image->decRef();
    }
}

IFont *OpenGLContext::createFont() { return new OpenGLFont(*this); }

void OpenGLContext::destroyFont(IFont *font) {
    if (font) {
        font->decRef();
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
