/*
 *  mkContext.cpp
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */
#include "mkContext.h"
#if defined(MNKVG_GLES_BACKEND)
#include "opengl_es/glesContext.h"
#include "opengl_es/glesPath.h"
#elif defined(MNKVG_GL_BACKEND)
#include "opengl/glContext.h"
#else
#error "No backend defined"
#endif

using namespace MonkVG;

VG_API_CALL VGboolean vgCreateContextMNK(VGint width, VGint height,
                                         VGRenderingBackendTypeMNK backend) {
    MK_LOG("Creating context %d, %d, %d", width, height, (int)backend);

    IContext::instance().setRenderingBackendType(backend);

    IContext::instance().setWidth(width);
    IContext::instance().setHeight(height);
    IContext::instance().Initialize();

    return VG_TRUE;
}

VG_API_CALL void vgResizeSurfaceMNK(VGint width, VGint height) {
    IContext::instance().setWidth(width);
    IContext::instance().setHeight(height);
    IContext::instance().resize();
}

VG_API_CALL void vgDestroyContextMNK() { IContext::instance().Terminate(); }

VG_API_CALL void VG_API_ENTRY vgSetf(VGuint type, VGfloat value) VG_API_EXIT {
    IContext::instance().set(type, value);
}

VG_API_CALL void VG_API_ENTRY vgSeti(VGuint type, VGint value) VG_API_EXIT {
    IContext::instance().set(type, value);
}

VG_API_CALL void VG_API_ENTRY vgSetfv(VGuint type, VGint count,
                                      const VGfloat *values) VG_API_EXIT {
    IContext::instance().set(type, values);
}
VG_API_CALL void VG_API_ENTRY vgSetiv(VGuint type, VGint count,
                                      const VGint *values) VG_API_EXIT {}

VG_API_CALL VGfloat VG_API_ENTRY vgGetf(VGuint type) VG_API_EXIT {
    VGfloat ret = -1;
    IContext::instance().get(type, ret);
    return ret;
}

VG_API_CALL VGint VG_API_ENTRY vgGeti(VGuint type) VG_API_EXIT {
    VGint ret = -1;
    IContext::instance().get(type, ret);
    return ret;
}

VG_API_CALL VGint VG_API_ENTRY vgGetVectorSize(VGuint type) VG_API_EXIT {
    return -1;
}

VG_API_CALL void VG_API_ENTRY vgGetfv(VGuint type, VGint count,
                                      VGfloat *values) VG_API_EXIT {}

VG_API_CALL void VG_API_ENTRY vgGetiv(VGuint type, VGint count,
                                      VGint *values) VG_API_EXIT {}

/* Masking and Clearing */
VG_API_CALL void VG_API_ENTRY vgClear(VGint x, VGint y, VGint width,
                                      VGint height) VG_API_EXIT {
    IContext::instance().clear(x, y, width, height);
}

VG_API_CALL void VG_API_ENTRY vgMask(VGHandle mask, VGMaskOperation operation,
                                     VGint x, VGint y, VGint width,
                                     VGint height) VG_API_EXIT {}

/* Finish and Flush */
VG_API_CALL void VG_API_ENTRY vgFinish(void) VG_API_EXIT {
    IContext::instance().finish();
}
VG_API_CALL void VG_API_ENTRY vgFlush(void) VG_API_EXIT {
    IContext::instance().flush();
}

/*--------------------------------------------------
 * Returns the oldest error pending on the current
 * context and clears its error code
 *--------------------------------------------------*/

VG_API_CALL VGErrorCode vgGetError(void) {
    return IContext::instance().getError();
}

VG_API_CALL void vgPushOrthoCamera(VGfloat left, VGfloat right, VGfloat bottom,
                                   VGfloat top, VGfloat near, VGfloat far) {
    IContext::instance().pushOrthoCamera(left, right, bottom, top, near, far);
}
VG_API_CALL void vgPopOrthoCamera() { IContext::instance().popOrthoCamera(); }

namespace MonkVG {

IContext::IContext() { setImageMode(_image_mode); }

//// parameters ////
void IContext::set(VGuint type, VGfloat f) {
    switch (type) {
    case VG_STROKE_LINE_WIDTH:
        setStrokeLineWidth(f);
        break;
    default:
        SetError(VG_ILLEGAL_ARGUMENT_ERROR);
        break;
    }
}

void IContext::set(VGuint type, const VGfloat *fv) {
    switch (type) {
    case VG_CLEAR_COLOR:
        setClearColor(fv);
        break;
    case VG_GLYPH_ORIGIN:
        setGlyphOrigin(fv);
        break;

    default:
        SetError(VG_ILLEGAL_ARGUMENT_ERROR);
        break;
    }
}

void IContext::set(VGuint type, VGint i) {

    switch (type) {
    case VG_MATRIX_MODE:
        setMatrixMode((VGMatrixMode)i);
        break;
    case VG_FILL_RULE:
        setFillRule((VGFillRule)i);
        break;
    case VG_TESSELLATION_ITERATIONS_MNK:
        setTessellationIterations(i);
        break;
    case VG_IMAGE_MODE:
        setImageMode((VGImageMode)i);
        break;
    default:
        break;
    }
}
void IContext::get(VGuint type, VGfloat &f) const {
    switch (type) {
    case VG_STROKE_LINE_WIDTH:
        f = getStrokeLineWidth();
        break;
    default:
        SetError(VG_ILLEGAL_ARGUMENT_ERROR);
        break;
    }
}

void IContext::get(VGuint type, VGfloat *fv) const {
    switch (type) {
    case VG_CLEAR_COLOR:
        getClearColor(fv);
        break;
    case VG_GLYPH_ORIGIN:
        getGlyphOrigin(fv);
        break;

    default:
        SetError(VG_ILLEGAL_ARGUMENT_ERROR);
        break;
    }
}
void IContext::get(VGuint type, VGint &i) const {
    i = -1;

    switch (type) {
    case VG_MATRIX_MODE:
        i = getMatrixMode();
        break;
    case VG_FILL_RULE:
        i = getFillRule();
        break;
    case VG_TESSELLATION_ITERATIONS_MNK:
        i = getTessellationIterations();
        break;
    case VG_IMAGE_MODE:
        i = getImageMode();
        break;
    case VG_SURFACE_WIDTH_MNK:
        i = getWidth();
        break;
    case VG_SURFACE_HEIGHT_MNK:
        i = getHeight();
        break;

    default:
        break;
    }
}

void IContext::setMatrixMode(VGMatrixMode mode) {
    _matrix_mode = mode;
    switch (mode) {
    case VG_MATRIX_PATH_USER_TO_SURFACE:
        _active_matrix = &_path_user_to_surface;
        break;
    case VG_MATRIX_IMAGE_USER_TO_SURFACE:
        _active_matrix = &_image_user_to_surface;
        break;
    case VG_MATRIX_GLYPH_USER_TO_SURFACE:
        _active_matrix = &_glyph_user_to_surface;
        break;
    default:
        SetError(VG_ILLEGAL_ARGUMENT_ERROR);
        break;
    }
}

void SetError(const VGErrorCode e) { IContext::instance().setError(e); }
} // namespace MonkVG
