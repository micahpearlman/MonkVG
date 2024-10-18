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
#include "vkColorPipeline.h"
#include "vkTexturePipeline.h"


namespace MonkVG {
//// singleton implementation ////
IContext &IContext::instance() {
    static VulkanContext g_context;
    return g_context;
}

VulkanContext::VulkanContext() : IContext() {}

bool VulkanContext::Initialize() {
    resize();
    return true;
}

bool VulkanContext::Terminate() {
    if (_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(_allocator);
        _allocator = VK_NULL_HANDLE;
    }
    if (_own_descriptor_pool && _descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(_logical_dev, _descriptor_pool, nullptr);
        _descriptor_pool = VK_NULL_HANDLE;
    }
    return true;
}

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

void VulkanContext::resize() {
    // setup the Vulkan viewport
    _viewport.x      = 0;
    _viewport.y      = 0;
    _viewport.width  = getWidth();
    _viewport.height = getHeight();

    // setup the Vulkan scissor
    _scissor.offset = {0, 0};
    _scissor.extent = {static_cast<uint32_t>(getWidth()),
                       static_cast<uint32_t>(getHeight())};
}

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

bool VulkanContext::setVulkanContext(VkInstance       instance,
                                     VkPhysicalDevice physical_device,
                                     VkDevice         logical_dev,
                                     VkRenderPass     render_pass,
                                     VkCommandBuffer  command_buffer,
                                     VkDescriptorPool descriptor_pool) {
    _logical_dev = logical_dev;
    _render_pass = render_pass;
    _instance    = instance;
    _phys_dev    = physical_device;

    // create the memory allocator
    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice         = physical_device;
    allocator_info.device                 = logical_dev;
    allocator_info.instance               = instance;
    vmaCreateAllocator(&allocator_info, &_allocator);
    if (_allocator == VK_NULL_HANDLE) {
        return false;
    }

    // if no descriptor pool is passed in then create one
    if (descriptor_pool == VK_NULL_HANDLE) {
        VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100},
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = 2;
        pool_info.pPoolSizes    = pool_sizes;
        pool_info.maxSets       = 100;

        if (vkCreateDescriptorPool(logical_dev, &pool_info, nullptr,
                                   &_descriptor_pool) != VK_SUCCESS) {
            return false;
        }
        _own_descriptor_pool = true;
    } else {
        _descriptor_pool = descriptor_pool;
    }

    // Define vertex input binding description
    VkVertexInputBindingDescription binding_desc = {};
    binding_desc.binding                         = 0;
    binding_desc.stride                          = sizeof(vertex_2d_t);
    binding_desc.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

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

    VkPipelineVertexInputStateCreateInfo color_vertex_state = {};
    color_vertex_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    color_vertex_state.vertexBindingDescriptionCount = 1;
    color_vertex_state.pVertexBindingDescriptions    = &binding_desc;
    color_vertex_state.vertexAttributeDescriptionCount =
        vertex_input_attribs.size();
    color_vertex_state.pVertexAttributeDescriptions =
        vertex_input_attribs.data();

    // _color_pipeline = std::make_unique<VulkanGraphicsPipeline>(
    //     *this, color_vert, sizeof(color_vert), color_frag, sizeof(color_frag),
    //     color_vertex_state);
    _color_pipeline = std::make_unique<ColorPipeline>(*this);

    /// Texture Pipeline
    // for the texture pipeline the vertex type is a 2d position and a 2d uv
    // VkVertexInputAttributeDescription uv_attrib = {};
    // uv_attrib.binding                           = 0;
    // uv_attrib.location                          = 1;
    // uv_attrib.format                            = VK_FORMAT_R32G32_SFLOAT;
    // uv_attrib.offset = offsetof(textured_vertex_2d_t, uv);
    // vertex_input_attribs.push_back(uv_attrib);

    // binding_desc.stride =
    //     sizeof(textured_vertex_2d_t); // reset the stride to be the sizeof
    //                                   // texture vertex

    // VkPipelineVertexInputStateCreateInfo texture_vertex_state = {};
    // texture_vertex_state.sType =
    //     VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // texture_vertex_state.vertexBindingDescriptionCount = 1;
    // texture_vertex_state.pVertexBindingDescriptions    = &binding_desc;
    // texture_vertex_state.vertexAttributeDescriptionCount =
    //     vertex_input_attribs.size();
    // texture_vertex_state.pVertexAttributeDescriptions =
    //     vertex_input_attribs.data();

    // _texture_pipeline = std::make_unique<VulkanGraphicsPipeline>(
    //     *this, texture_vert, sizeof(texture_vert), texture_frag,
    //     sizeof(texture_frag), texture_vertex_state);
    return true;
}

} // namespace MonkVG

VG_API_CALL VGboolean vgSetVulkanContextMNK(void *instance,
                                            void *physical_device,
                                            void *logical_device,
                                            void *render_pass,
                                            void *command_buffer,
                                            void *descriptor_pool) {
    MonkVG::VulkanContext &vk_ctx =
        (MonkVG::VulkanContext &)MonkVG::IContext::instance();
    vk_ctx.setVulkanContext((VkInstance)instance,
                            (VkPhysicalDevice)physical_device,
                            (VkDevice)logical_device, (VkRenderPass)render_pass,
                            (VkCommandBuffer)command_buffer,
                            (VkDescriptorPool)descriptor_pool);

    return VG_TRUE;
}
