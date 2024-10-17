/* $Revision: 6810 $ on $Date:: 2008-10-29 15:31:37 +0100 #$ */

/*------------------------------------------------------------------------
 *
 * VG extensions Reference Implementation
 * -------------------------------------
 *
 * Copyright (c) 2008 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//**
 * \file
 * \brief	VG extensions
 *//*-------------------------------------------------------------------*/

#ifndef _VGEXT_H
#define _VGEXT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "openvg.h"
#include "vgu.h"

#ifndef VG_API_ENTRYP
#define VG_API_ENTRYP VG_API_ENTRY *
#endif

#ifndef VGU_API_ENTRYP
#define VGU_API_ENTRYP VGU_API_ENTRY *
#endif

/*-------------------------------------------------------------------------------
 * MonkVG extensions
 *------------------------------------------------------------------------------*/

/*	set the number of iterations for tesselating curves.  a low number will
 *generate less polygons but rougher looking rendering -- and vice versa.
 */
typedef enum {
    VG_TESSELLATION_ITERATIONS_MNK = 0x1170,

    /* the rendering buffer dimensions */
    VG_SURFACE_WIDTH_MNK  = 0x1171,
    VG_SURFACE_HEIGHT_MNK = 0x1172,

    VG_PARAM_TYPE_MNK_FORCE_SIZE = VG_MAX_ENUM
} VGParamTypeMNK;

/**
 * @brief Rendering backend types.
 * NOTE: Need to compile with the correct backend type.
 * See: CMakeLists.txt for details.
 *
 */
typedef enum {
    VG_RENDERING_BACKEND_TYPE_OPENGLES11 = 0,
    VG_RENDERING_BACKEND_TYPE_OPENGLES20 = 1,
    VG_RENDERING_BACKEND_TYPE_OPENGLES32 = 2,
    VG_RENDERING_BACKEND_TYPE_OPENGL33   = 3,
    VG_RENDERING_BACKEND_TYPE_VULKAN     = 4,
    VG_RENDERING_BACKEND_TYPE_FORCE_SIZE = VG_MAX_ENUM
} VGRenderingBackendTypeMNK;

/* batches are a method for significantly speeding up rendering of collections
 * of static paths
 */
typedef VGHandle VGBatchMNK;

VG_API_CALL VGBatchMNK VG_API_ENTRY vgCreateBatchMNK() VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgDestroyBatchMNK(VGBatchMNK batch) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgBeginBatchMNK(VGBatchMNK batch) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgEndBatchMNK(VGBatchMNK batch) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgDrawBatchMNK(VGBatchMNK batch) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgDumpBatchMNK(VGBatchMNK batch, void **vertices,
                                             size_t *size) VG_API_EXIT;

/**
 * @brief Creates a MonkVG context with the specified rendering backend.
 *
 * @param width width of the rendering surface
 * @param height height of the rendering surface
 * @param backend the rendering backend type.  NOTE: Must be compiled with the
 * correct backend type.
 * @return VG_API_CALL
 */
VG_API_CALL VGboolean vgCreateContextMNK(VGint width, VGint height,
                                         VGRenderingBackendTypeMNK backend);

/**
 * @brief Handle surface resize events.
 *
 * @param width new surface width
 * @param height new surface height
 * @return * VG_API_CALL
 */
VG_API_CALL void vgResizeSurfaceMNK(VGint width, VGint height);

/**
 * @brief Cleans up MonkVG context.
 *
 * @return VG_API_CALL
 */
VG_API_CALL void vgDestroyContextMNK(void);

/**
 * @brief Set Vulkan context for MonkVG.
 *
 * @param instance The Vulkan instance
 * @param physical_device The Vulkan physical device
 * @param logical_device The Vulkan logical device
 * @param render_pass The Vulkan render pass
 * @return VG_API_CALL
 */
VG_API_CALL VGboolean vgSetVulkanContextMNK(void *instance,
                                            void *physical_device,
                                            void *logical_device,
                                            void *render_pass);

/* Helper function for things like camera
 */

/**
 * @brief Push an orthographic camera onto the matrix stack.
 * NOTE: OpenVG has 0,0 as the bottom left corner.
 *
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near
 * @param far
 * @return VG_API_CALL
 */
VG_API_CALL void vgPushOrthoCamera(VGfloat left, VGfloat right, VGfloat bottom,
                                   VGfloat top, VGfloat near, VGfloat far);

/**
 * @brief Pop the orthographic camera off the matrix stack.
 *
 * @return * VG_API_CALL
 */
VG_API_CALL void vgPopOrthoCamera();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _VGEXT_H */
