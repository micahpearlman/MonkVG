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
#include "vkPaint.h"
#include "vkImage.h"

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
    // destroy the pipelines
    _color_triangle_pipeline.reset();
    _color_tristrip_pipeline.reset();
    _texture_triangle_pipeline.reset();
    _texture_tristrip_pipeline.reset();

    if (_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(_allocator);
        _allocator = VK_NULL_HANDLE;
    }

    // if we own the descriptor pool then destroy it
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
    if (path == nullptr) {
        SetError(VG_OUT_OF_MEMORY_ERROR);
    }

    return (IPath *)path;
}

void VulkanContext::destroyPath(IPath *path) {
    if (path) {
        delete path;
    }
}

IPaint *VulkanContext::createPaint() {
    VulkanPaint *paint = new VulkanPaint(*this);
    if (paint == nullptr) {
        SetError(VG_OUT_OF_MEMORY_ERROR);
    }
    return (IPaint *)paint;
}

void VulkanContext::destroyPaint(IPaint *paint) {
    if (paint) {
        delete paint;
    }
}

IImage *VulkanContext::createImage(VGImageFormat format, VGint width,
                                   VGint height, VGbitfield allowedQuality) {
    return new VulkanImage(format, width, height, allowedQuality, *this);
}

void VulkanContext::destroyImage(IImage *image) {
    if (image) {
        delete image;
    }
}

IBatch *VulkanContext::createBatch() { return nullptr; }

void VulkanContext::destroyBatch(IBatch *batch) {
    if (batch) {
        delete batch;
    }
}

IFont *VulkanContext::createFont() { return nullptr; }

void VulkanContext::destroyFont(IFont *font) {
    if (font) {
        delete font;
    }
}

void VulkanContext::stroke() {}

void VulkanContext::fill() {}

void VulkanContext::clear(VGint x, VGint y, VGint width, VGint height) {}

void VulkanContext::flush() {}

void VulkanContext::finish() {}

void VulkanContext::resize() {
    // setup the Vulkan viewport
    _viewport.x        = 0;
    _viewport.y        = 0;
    _viewport.width    = getWidth();
    _viewport.height   = getHeight();
    _viewport.minDepth = 0.0f;
    _viewport.maxDepth = 1.0f;

    // setup the Vulkan scissor
    _scissor.offset = {0, 0};
    _scissor.extent = {static_cast<uint32_t>(getWidth()),
                       static_cast<uint32_t>(getHeight())};

    // set the orthographic projection
    pushOrthoCamera(0, getWidth(), 0, getHeight(), -1, 1);
}

void VulkanContext::startBatch(IBatch *batch) {}

void VulkanContext::dumpBatch(IBatch *batch, void **vertices, size_t *size) {}

void VulkanContext::endBatch(IBatch *batch) {}

bool VulkanContext::setVulkanContext(
    VkInstance instance, VkPhysicalDevice physical_device, VkDevice logical_dev,
    VkRenderPass render_pass, VkCommandBuffer command_buffer,
    VkCommandPool command_pool, VkQueue graphics_queue,
    VkDescriptorPool descriptor_pool) {
    _logical_dev    = logical_dev;
    _render_pass    = render_pass;
    _instance       = instance;
    _phys_dev       = physical_device;
    _command_buffer = command_buffer;
    _command_pool   = command_pool;
    _graphics_queue = graphics_queue;

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

    _color_triangle_pipeline = std::make_unique<ColorPipeline>(
        *this, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    _color_tristrip_pipeline = std::make_unique<ColorPipeline>(
        *this, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

    _texture_triangle_pipeline = std::make_unique<TexturePipeline>(
        *this, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    _texture_tristrip_pipeline = std::make_unique<TexturePipeline>(
        *this, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

    return true;
}

VkCommandBuffer VulkanContext::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = _command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(_logical_dev, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    return command_buffer;
}

void VulkanContext::endSingleTimeCommands(VkCommandBuffer command_buffer) {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info       = {};
    submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &command_buffer;

    if (vkQueueSubmit(_graphics_queue, 1, &submit_info, VK_NULL_HANDLE) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "failed to submit command buffer for single time command");
    }
    vkQueueWaitIdle(_graphics_queue);

    vkFreeCommandBuffers(_logical_dev, _command_pool, 1, &command_buffer);
}

} // namespace MonkVG

VG_API_CALL VGboolean vgSetVulkanContextMNK(
    VkInstance instance, VkPhysicalDevice physical_device,
    VkDevice logical_device, VkRenderPass render_pass,
    VkCommandBuffer command_buffer, VkCommandPool command_pool,
    VkQueue graphics_queue, VkDescriptorPool descriptor_pool) {
    MonkVG::VulkanContext &vk_ctx =
        (MonkVG::VulkanContext &)MonkVG::IContext::instance();
    vk_ctx.setVulkanContext(instance, physical_device, logical_device,
                            render_pass, command_buffer, command_pool,
                            graphics_queue, descriptor_pool);

    return VG_TRUE;
}