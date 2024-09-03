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
#undef CHECK_GL_ERROR // gles2-bc also leaves this defined.

namespace MonkVG {

// todo: setup debug and release versions
// #define CHECK_GL_ERROR OpenGLContext::checkGLError()
#define CHECK_GL_ERROR
class OpenGLContext : public IContext {
  public:
    OpenGLContext();

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

    //// platform specific implementation of transform ////
    void setIdentity() override;
    void transform(VGfloat *t) override;
    void scale(VGfloat sx, VGfloat sy) override;
    void translate(VGfloat x, VGfloat y) override;
    void rotate(VGfloat angle) override;
    void setTransform(const VGfloat *t) override;
    void multiply(const VGfloat *t) override;
    void setMatrixMode(VGMatrixMode mode) override {
        IContext::setMatrixMode(mode);
        loadGLMatrix();
    }

    /// batch drawing override
    void startBatch(IBatch *batch) override;
    void dumpBatch(IBatch *batch, void **vertices, size_t *size) override;
    void endBatch(IBatch *batch) override;

    /// image
    void setImageMode(VGImageMode im) override;

    /// camera extension
    void pushOrthoCamera(VGfloat left, VGfloat right, VGfloat bottom,
                                 VGfloat top, VGfloat near, VGfloat far)  override;
    void popOrthoCamera() override;

    void resize() override;

    /// OpenGL specific
    void loadGLMatrix();
    void beginRender();
    void endRender();    
    static void checkGLError();

  private:
    // restore values to play nice with other apps
    int   _viewport[4];
    float _projection[16];
    float _modelview[16];
    float _color[4];
};
} // namespace MonkVG

#endif // __qlContext_h__
