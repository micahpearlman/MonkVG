/**
 * @file vkContext.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan context management implementation. Singleton implementation.
 * @version 0.1
 * @date 2024-10-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "vkContext.h"
#include "vkPath.h"

namespace MonkVG {
//// singleton implementation ////
IContext &IContext::instance() {
    static VulkanContext g_context;
    return g_context;
}


VulkanContext::VulkanContext() : IContext() {}

bool VulkanContext::Initialize() {
    return true;
}

bool VulkanContext::Terminate() {
    return true;
}

IPath *VulkanContext::createPath(VGint path_format, VGPathDatatype datatype, VGfloat scale,
                                 VGfloat bias, VGint segment_capacity_hint,
                                 VGint coord_capacity_hint, VGbitfield capabilities) {
    VulkanPath *path = new VulkanPath(
        path_format, datatype, scale, bias, segment_capacity_hint,
        coord_capacity_hint, capabilities &= VG_PATH_CAPABILITY_ALL, *this);
    if (path == 0) {
        SetError(VG_OUT_OF_MEMORY_ERROR);
    }

    return (IPath *)path;
}

void VulkanContext::destroyPath(IPath *path) {}

IPaint *VulkanContext::createPaint() {
    return nullptr;
}

void VulkanContext::destroyPaint(IPaint *paint) {}

IImage *VulkanContext::createImage(VGImageFormat format, VGint width, VGint height,
                                   VGbitfield allowedQuality) {
    return nullptr;
}

void VulkanContext::destroyImage(IImage *image) {}

IBatch *VulkanContext::createBatch() {
    return nullptr;
}

void VulkanContext::destroyBatch(IBatch *batch) {}

IFont *VulkanContext::createFont() {
    return nullptr;
}

void VulkanContext::destroyFont(IFont *font) {}

void VulkanContext::stroke() {}

void VulkanContext::fill() {}

void VulkanContext::clear(VGint x, VGint y, VGint width, VGint height) {}

void VulkanContext::flush() {}

void VulkanContext::finish() {}

void VulkanContext::resize() {}

void VulkanContext::setIdentity() {}

void VulkanContext::transform(VGfloat *t) {}

void VulkanContext::scale(VGfloat sx, VGfloat sy) {}

void VulkanContext::translate(VGfloat x, VGfloat y) {}

void VulkanContext::rotate(VGfloat angle) {}

void VulkanContext::setTransform(const VGfloat *t) {}

void VulkanContext::multiply(const VGfloat *t) {}

void VulkanContext::startBatch(IBatch *batch) {}

void VulkanContext::dumpBatch(IBatch *batch, void **vertices, size_t *size) {}

void VulkanContext::endBatch(IBatch *batch) {}

void VulkanContext::pushOrthoCamera(VGfloat left, VGfloat right, VGfloat bottom,
                                    VGfloat top, VGfloat near, VGfloat far) {}

void VulkanContext::popOrthoCamera() {}


} // namespace MonkVG
