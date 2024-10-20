/*
 *  glContext.h
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#ifndef __glContext_h__
#define __glContext_h__

#include "mkContext.h"
#include "glPlatform.h"
#include "glShader.h"
#include <glm/glm.hpp>
#include <stack>
namespace MonkVG {

// turn on GL error checking if debug
#if !defined(NDEBUG)
#define CHECK_GL_ERROR OpenGLContext::checkGLError()
#else
#define CHECK_GL_ERROR
#endif

/**
 * @brief Context implementation for OpenGL. Contains OpenGL specific
 * implementations for the IContext interface, as well as OpenGL specific
 * methods for setting up the rendering context. Such as matrices, shaders, etc.
 *
 */
class OpenGLContext : public IContext {
  public:
    OpenGLContext();
    virtual ~OpenGLContext() = default;

    bool Initialize() override;
    bool Terminate() override;

    //// factories ////
    IPath  *createPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
                       VGfloat bias, VGint segmentCapacityHint,
                       VGint      coordCapacityHint,
                       VGbitfield capabilities) override;
    void    destroyPath(IPath *path) override;
    void    destroyPaint(IPaint *paint) override;
    IPaint *createPaint() override;
    IImage *createImage(VGImageFormat format, VGint width, VGint height,
                        VGbitfield allowedQuality) override;
    void    destroyImage(IImage *image) override;
    IBatch *createBatch() override;
    void    destroyBatch(IBatch *batch) override;
    IFont  *createFont() override;
    void    destroyFont(IFont *font) override;

    /// paint overrides
    void setStrokePaint(IPaint *paint) override;
    void setFillPaint(IPaint *paint) override;

    //// platform specific execution of stroke and fill ////
    void stroke() override;
    void fill() override;

    //// platform specific execution of Masking and Clearing ////
    void clear(VGint x, VGint y, VGint width, VGint height) override;
    void flush() override;
    void finish() override;


    /// batch drawing override
    void startBatch(IBatch *batch) override;
    void dumpBatch(IBatch *batch, void **vertices, size_t *size) override;
    void endBatch(IBatch *batch) override;

    /// image
    void setImageMode(VGImageMode im) override;


    void resize() override;


    /// shader management
    enum ShaderType { ColorShader, TextureShader, GradientShader, None };

    /**
     * @brief bind the shader for the given type.  This will also setup
     * common uniforms for the shader.
     *
     * @param shader
     */
    void          bindShader(ShaderType shader);
    ShaderType    getCurrentShaderType() const { return _current_shader; }
    OpenGLShader &getCurrentShader();

    static void checkGLError();

  private:
    // restore values to play nice with other apps
    int _restore_viewport[4];

    std::unique_ptr<OpenGLShader> _color_shader;
    std::unique_ptr<OpenGLShader> _texture_shader;
    std::unique_ptr<OpenGLShader> _gradient_shader;


    ShaderType _current_shader = ShaderType::None;
};
} // namespace MonkVG

#endif // __qlContext_h__
