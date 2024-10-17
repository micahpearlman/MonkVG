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

const static uint32_t color_vert[] =
#include "shaders/color.vert.h"
    ;

const static uint32_t color_frag[] =
#include "shaders/color.frag.h"
    ;

const static uint32_t texture_vert[] =
#include "shaders/texture.vert.h"
    ;

const static uint32_t texture_frag[] =
#include "shaders/texture.frag.h"
    ;

namespace MonkVG {
//// singleton implementation ////
IContext &IContext::instance() {
    static VulkanContext g_context;
    return g_context;
}

VulkanContext::VulkanContext() : IContext() {}

bool VulkanContext::Initialize() { return true; }

bool VulkanContext::Terminate() { return true; }

IPath *VulkanContext::createPath(VGint path_format, VGPathDatatype datatype,
                                 VGfloat scale, VGfloat bias,
                                 VGint      segment_capacity_hint,
                                 VGint      coord_capacity_hint,
                                 VGbitfield capabilities) {
    VulkanPath *path = new VulkanPath(
        path_format, datatype, scale, bias, segment_capacity_hint,
        coord_capacity_hint, capabilities &= VG_PATH_CAPABILITY_ALL, *this);
    if (path == 0) {
        SetError(VG_OUT_OF_MEMORY_ERROR);
    }

    return (IPath *)path;
}

void VulkanContext::destroyPath(IPath *path) {}

IPaint *VulkanContext::createPaint() { return nullptr; }

void VulkanContext::destroyPaint(IPaint *paint) {}

IImage *VulkanContext::createImage(VGImageFormat format, VGint width,
                                   VGint height, VGbitfield allowedQuality) {
    return nullptr;
}

void VulkanContext::destroyImage(IImage *image) {}

IBatch *VulkanContext::createBatch() { return nullptr; }

void VulkanContext::destroyBatch(IBatch *batch) {}

IFont *VulkanContext::createFont() { return nullptr; }

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

bool VulkanContext::setVulkanContext(VkDevice logical_dev) {
    _logical_dev = logical_dev;

    // Create the graphics pipelines
    /// Color Pipeline
    // for the color pipeline the vertex type is just a 2d position
    // the color is a uniform
    std::vector<VkVertexInputAttributeDescription> vertex_input_attribs = {};
    VkVertexInputAttributeDescription              vertex_attrib        = {};
    vertex_attrib.binding                                               = 0;
    vertex_attrib.location                                              = 0;
    vertex_attrib.format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attrib.offset = offsetof(vertex_2d_t, v);
    vertex_input_attribs.push_back(vertex_attrib);
    _color_pipeline = std::make_unique<VulkanGraphicsPipeline>(
        *this, color_vert, sizeof(color_vert), color_frag, sizeof(color_frag),
        vertex_input_attribs);
    /// Texture Pipeline
    // for the texture pipeline the vertex type is a 2d position and a 2d uv
    VkVertexInputAttributeDescription uv_attrib = {};
    uv_attrib.binding                           = 0;
    uv_attrib.location                          = 1;
    uv_attrib.format                            = VK_FORMAT_R32G32_SFLOAT;
    uv_attrib.offset = offsetof(textured_vertex_2d_t, uv);
    vertex_input_attribs.push_back(uv_attrib);
    _texture_pipeline = std::make_unique<VulkanGraphicsPipeline>(
        *this, texture_vert, sizeof(texture_vert), texture_frag,
        sizeof(texture_frag), vertex_input_attribs);
    return true;
}

} // namespace MonkVG

VG_API_CALL VGboolean vgSetVulkanContextMNK(void *logical_device) {
    MonkVG::VulkanContext &vk_ctx =
        (MonkVG::VulkanContext &)MonkVG::IContext::instance();
    vk_ctx.setVulkanContext((VkDevice)logical_device);

    return VG_TRUE;
}
