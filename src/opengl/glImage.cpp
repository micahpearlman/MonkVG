//
//  glImage.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include "glImage.h"
#include "glContext.h"

namespace MonkVG {

OpenGLImage::OpenGLImage(VGImageFormat format, VGint width, VGint height,
                         VGbitfield allowedQuality)
    : IImage(format, width, height, allowedQuality) {
    CHECK_GL_ERROR;
    // create the texture
    glGenTextures(1, &_gl_texture);
    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECK_GL_ERROR;

    //		/* RGB{A,X} channel ordering */
    //		VG_sRGBX_8888                               =  0,
    //		VG_sRGBA_8888                               =  1,
    //		VG_sRGBA_8888_PRE                           =  2,
    //		VG_sRGB_565                                 =  3,
    //		VG_sRGBA_5551                               =  4,
    //		VG_sRGBA_4444                               =  5,
    //		VG_sL_8                                     =  6,
    //		VG_lRGBX_8888                               =  7,
    //		VG_lRGBA_8888                               =  8,
    //		VG_lRGBA_8888_PRE                           =  9,
    //		VG_lL_8                                     = 10,
    //		VG_A_8                                      = 11,
    //		VG_BW_1                                     = 12,
    //		VG_A_1                                      = 13,
    //		VG_A_4                                      = 14,

    switch (format) {

    case VG_sRGBA_8888:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, 0);
        break;
    case VG_sRGB_565:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_SHORT_5_6_5, 0);
        break;
    case VG_A_8:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA,
                     GL_UNSIGNED_BYTE, 0);
        break;
    default:
        SetError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
        throw std::runtime_error("VG_UNSUPPORTED_IMAGE_FORMAT_ERROR");
        break;
    }

    CHECK_GL_ERROR;

    // create the vertex array & buffer objects
    const GLfloat w = (GLfloat)_width;
    const GLfloat h = (GLfloat)_height;
    const GLfloat x = 0, y = 0;
    // NOTE: openvg coordinate system is bottom, left is 0,0
    // clang-format off
    std::array<GLfloat, 16> vertices = {
        x,     y,     _s[0], _t[1], // left, bottom
        x + w, y,     _s[1], _t[1], // right, bottom
        x,     y + h, _s[0], _t[0], // left, top
        x + w, y + h, _s[1], _t[0]  // right, top
    };
    // clang-format on

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(),
                 GL_DYNAMIC_DRAW); // we will be updating vertices at every draw
                                   //  GL_STATIC_DRAW); // we will not be
                                   //  updating vertices
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                          (GLvoid *)0); // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                          (GLvoid *)(2 * sizeof(GLfloat))); // texcoord
    glEnableVertexAttribArray(1);
    CHECK_GL_ERROR;
}

OpenGLImage::OpenGLImage(OpenGLImage &other)
    : IImage(other),
      _gl_texture(other._gl_texture),
      _vao(other._vao),
      _vbo(other._vbo) {}

OpenGLImage::~OpenGLImage() {
    CHECK_GL_ERROR;

    // if this is a child image then don't delete the texture
    if (!_parent && _gl_texture != GL_UNDEFINED) {
        glDeleteTextures(1, &_gl_texture);
        _gl_texture = GL_UNDEFINED;
    }

    if (!_parent && _vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_vbo);
        _vbo = GL_UNDEFINED;
    }
    if (!_parent && _vao != GL_UNDEFINED) {
        glDeleteVertexArrays(1, &_vao);
        _vao = GL_UNDEFINED;
    }
    CHECK_GL_ERROR;
}

IImage *OpenGLImage::createChild(VGint x, VGint y, VGint w, VGint h) {
    OpenGLImage *glImage = new OpenGLImage(*this);
    glImage->_s[0]       = VGfloat(x) / VGfloat(_width);
    glImage->_s[1]       = VGfloat(x + w) / VGfloat(_width);
    glImage->_t[0]       = VGfloat(y) / VGfloat(_height);
    glImage->_t[1]       = VGfloat(y + h) / VGfloat(_height);
    glImage->_width      = w;
    glImage->_height     = h;

    return glImage;
}

void OpenGLImage::setSubData(const void *data, VGint dataStride,
                             VGImageFormat dataFormat, VGint x, VGint y,
                             VGint width, VGint height) {
    CHECK_GL_ERROR;

    glBindTexture(GL_TEXTURE_2D, _gl_texture);

    switch (dataFormat) {

    case VG_sRGBA_8888:
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA,
                        GL_UNSIGNED_BYTE, data);
        break;
    case VG_sRGB_565:
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB,
                        GL_UNSIGNED_SHORT_5_6_5, data);
        break;
    case VG_A_8:
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_ALPHA,
                        GL_UNSIGNED_BYTE, data);
        break;
    default:
        SetError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
        assert(0);
        break;
    }
    CHECK_GL_ERROR;
}

void OpenGLImage::draw() {
    CHECK_GL_ERROR;

    if (IContext::instance().getImageMode() == VG_DRAW_IMAGE_MULTIPLY) {
        // set the color to the current fill paint color
        IPaint *fillPaint = IContext::instance().getFillPaint();
        const std::array<VGfloat, 4> color = fillPaint->getPaintColor();
        throw std::runtime_error("VG_DRAW_IMAGE_MULTIPLY not implemented");
    }

    // draw the image
    const GLfloat w = (GLfloat)_width;
    const GLfloat h = (GLfloat)_height;
    const GLfloat x = 0;
    const GLfloat y = 0;

    // NOTE: openvg coordinate system is bottom, left is 0,0
    // clang-format off
    std::array<GLfloat,16> vertices = {
			x,     y,      _s[0], _t[1],  	// left, bottom
			x + w, y,      _s[1], _t[1],	// right, bottom
			x,     y + h,  _s[0], _t[0],	// left, top
			x + w, y + h,  _s[1], _t[0] 	// right, top
	};
    // clang-format on

    // bind texture
    bind();

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat),
                    vertices.data());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    unbind();

    CHECK_GL_ERROR;
}

void OpenGLImage::drawSubRect(VGint ox, VGint oy, VGint w, VGint h,
                              VGbitfield paintModes) {
    CHECK_GL_ERROR;
    if (ox < 0 || oy < 0 || w < 0 || h < 0 || ox + w > _width ||
        oy + h > _height) {
        SetError(VG_ILLEGAL_ARGUMENT_ERROR);
        return;
    }
    
    GLfloat minS = GLfloat(ox) / GLfloat(_width);
    GLfloat maxS = GLfloat(ox + w) / GLfloat(_width);
    GLfloat minT = GLfloat(oy) / GLfloat(_height);
    GLfloat maxT = GLfloat(oy + h) / GLfloat(_height);


    GLfloat x = 0, y = 0;

    // clang-format off
    std::array<GLfloat,16> vertices = {
						x,     y,      minS, maxT,  // left, bottom
                        x + w, y,      maxS, maxT,	// right, bottom
                        x,     y + h,  minS, minT,	// left, top
                        x + w, y + h,  maxS, minT 	// right, top
	};

    // clang-format on

    if (IContext::instance().getImageMode() == VG_DRAW_IMAGE_MULTIPLY) {
        // set the color to the current fill paint color
        IPaint *fillPaint = IContext::instance().getFillPaint();
        const std::array<VGfloat, 4> color = fillPaint->getPaintColor();
        throw std::runtime_error("VG_DRAW_IMAGE_MULTIPLY not implemented");
    }

    bind();
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat),
                    vertices.data());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    unbind();
    CHECK_GL_ERROR;
}

void OpenGLImage::drawToRect(VGint x_, VGint y_, VGint w_, VGint h_,
                             VGbitfield paintModes) {
    CHECK_GL_ERROR;

    GLfloat x = (GLfloat)x_;
    GLfloat y = (GLfloat)y_;
    GLfloat w = (GLfloat)w_;
    GLfloat h = (GLfloat)h_;
    // clang-format off
    std::array<GLfloat,16> vertices = {
			x,     y,      _s[0], _t[1],  	// left, bottom
			x + w, y,      _s[1], _t[1],	// right, bottom
			x,     y + h,  _s[0], _t[0],	// left, top
			x + w, y + h,  _s[1], _t[0] 	// right, top
	};
    // clang-format on

    if (IContext::instance().getImageMode() == VG_DRAW_IMAGE_MULTIPLY) {
        // set the color to the current fill paint color
        IPaint *fillPaint = IContext::instance().getFillPaint();
        const std::array<VGfloat, 4> color = fillPaint->getPaintColor();
        throw std::runtime_error("VG_DRAW_IMAGE_MULTIPLY not implemented");
    }

    bind();

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat),
                    vertices.data());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    unbind();
    CHECK_GL_ERROR;
}

void OpenGLImage::drawAtPoint(VGint x, VGint y, VGbitfield paintModes) {
    drawToRect(x, y, _width, _height, paintModes);
}

void OpenGLImage::bind() {
    CHECK_GL_ERROR;
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    OpenGLContext &gl_ctx = (MonkVG::OpenGLContext &)IContext::instance();
    gl_ctx.bindShader(OpenGLContext::ShaderType::TextureShader);

    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glBindVertexArray(_vao);
    CHECK_GL_ERROR;
}
void OpenGLImage::unbind() {
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    OpenGLContext &gl_ctx = (MonkVG::OpenGLContext &)IContext::instance();
    gl_ctx.bindShader(OpenGLContext::ShaderType::None);
    CHECK_GL_ERROR;
}

} // namespace MonkVG
