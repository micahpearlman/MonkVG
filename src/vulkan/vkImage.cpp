/**
 * @file vkImage.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan OpenVG Image implementation
 * @version 0.1
 * @date 2024-10-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vkImage.h"
#include "vkContext.h"
#include "vkTexturePipeline.h"
#include "vkColorPipeline.h"
#include <cstring>
namespace MonkVG {

VulkanImage::VulkanImage(VGImageFormat format, VGint width, VGint height,
                         VGbitfield allowedQuality, IContext &context)
    : IImage(format, width, height, allowedQuality, context) {
    VkFormat vk_format = VK_FORMAT_R8G8B8A8_UNORM;
    switch (format) {
    case VG_sRGBA_8888:
        vk_format         = VK_FORMAT_R8G8B8A8_UNORM;
        _pixel_size_bytes = 4;
        break;
    case VG_sRGB_565:
        vk_format         = VK_FORMAT_R5G6B5_UNORM_PACK16;
        _pixel_size_bytes = 2;
        break;
    case VG_A_8:
        vk_format         = VK_FORMAT_R8_UNORM;
        _pixel_size_bytes = 1;
        break;
    default:
        SetError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
        throw std::runtime_error("VG_UNSUPPORTED_IMAGE_FORMAT_ERROR");
    }
    VkImageCreateInfo image_info = {};
    image_info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType         = VK_IMAGE_TYPE_2D;
    image_info.extent.width      = width;
    image_info.extent.height     = height;
    image_info.extent.depth      = 1;
    image_info.mipLevels         = 1;
    image_info.arrayLayers       = 1;
    image_info.format            = vk_format;
    image_info.tiling            = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage =
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples     = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(getVulkanContext().getVulkanAllocator(), &image_info,
                       &alloc_info, &_image, &_image_allocation,
                       nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image");
    }

    // create image view
    VkImageViewCreateInfo view_info = {};
    view_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image                 = _image;
    view_info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format                = vk_format;
    view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel   = 0;
    view_info.subresourceRange.levelCount     = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(getVulkanContext().getVulkanLogicalDevice(),
                          &view_info, nullptr, &_image_view) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view");
    }

    /// create vertex buffer
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size  = sizeof(textured_vertex_2d_t) * 4; // 4 vertices
    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vertex_alloc_info = {};
    vertex_alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(getVulkanContext().getVulkanAllocator(), &buffer_info,
                        &vertex_alloc_info, &_vertex_buffer,
                        &_vertex_buffer_allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image vertex buffer");
    }
}

VulkanImage::VulkanImage(VulkanImage &other) : IImage(other) {
    _image                    = other._image;
    _image_allocation         = other._image_allocation;
    _pixel_size_bytes         = other._pixel_size_bytes;
    _vertex_buffer            = other._vertex_buffer;
    _vertex_buffer_allocation = other._vertex_buffer_allocation;
}

VulkanImage::~VulkanImage() {
    if (!getParent() && _image != VK_NULL_HANDLE) {
        vmaDestroyImage(getVulkanContext().getVulkanAllocator(), _image,
                        _image_allocation);
    }
    if (!getParent() && _vertex_buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(),
                         _vertex_buffer, _vertex_buffer_allocation);
    }
}

IImage *VulkanImage::createChild(VGint x, VGint y, VGint w, VGint h) {
    VulkanImage *child_image = new VulkanImage(*this);
    child_image->_s[0]       = VGfloat(x) / VGfloat(_width);
    child_image->_s[1]       = VGfloat(x + w) / VGfloat(_width);
    child_image->_t[0]       = VGfloat(y) / VGfloat(_height);
    child_image->_t[1]       = VGfloat(y + h) / VGfloat(_height);
    child_image->_width      = w;
    child_image->_height     = h;

    return child_image;
}

void VulkanImage::setSubData(const void *data, VGint dataStride,
                             VGImageFormat dataFormat, VGint x, VGint y,
                             VGint width, VGint height) {

    if (dataFormat != getFormat()) {
        throw std::runtime_error("data format does not match image format");
        return;
    }
    // upload the image data to the GPU
    VkDeviceSize  image_size = width * height * getPixelSizeBytes();
    VkBuffer      staging_buffer;
    VmaAllocation staging_buffer_allocation;

    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size               = image_size;
    buffer_info.usage              = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_ONLY;

    if (vmaCreateBuffer(getVulkanContext().getVulkanAllocator(), &buffer_info,
                        &alloc_info, &staging_buffer,
                        &staging_buffer_allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create staging buffer");
    }

    void *staging_data;
    vmaMapMemory(getVulkanContext().getVulkanAllocator(),
                 staging_buffer_allocation, &staging_data);
    memcpy(staging_data, data, image_size);
    vmaUnmapMemory(getVulkanContext().getVulkanAllocator(),
                   staging_buffer_allocation);

    // Upload the image data to the GPU using a command buffer
    VkCommandBuffer command_buffer =
        getVulkanContext().beginSingleTimeCommands();
    {

        // setup the start of the image barrier
        VkImageMemoryBarrier barrier = {};
        barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout            = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                = _image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = 0;
        barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                             nullptr, 1, &barrier);

        // copy the image data
        VkBufferImageCopy region               = {};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = {x, y, 0};
        region.imageExtent = {uint32_t(width), uint32_t(height), 1};

        vkCmdCopyBufferToImage(command_buffer, staging_buffer, _image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                               &region);

        // setup the end of the image barrier
        barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);
    }
    getVulkanContext().endSingleTimeCommands(command_buffer);

    // cleanup buffers
    vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(), staging_buffer,
                     staging_buffer_allocation);

    // {
    //     const float w = (float)_width;
    //     const float h = (float)_height;
    //     const float x = 0;
    //     const float y = 0;

    //     std::array<textured_vertex_2d_t, 4> vertices = {{
    //         {x, y, _s[0], _t[1]},        // left, bottom
    //         {x + w, y, _s[1], _t[1]},    // right, bottom
    //         {x, y + h, _s[0], _t[0]},    // left, top
    //         {x + w, y + h, _s[1], _t[0]} // right, top
    //     }};

    //     // clang-format on

    //     // copy the vertex data to the buffer
    //     void *data;
    //     vmaMapMemory(getVulkanContext().getVulkanAllocator(),
    //                  _vertex_buffer_allocation, &data);
    //     memcpy(data, vertices.data(),
    //            vertices.size() * sizeof(textured_vertex_2d_t));
    //     vmaUnmapMemory(getVulkanContext().getVulkanAllocator(),
    //                    _vertex_buffer_allocation);
    // }
    // getVulkanContext().getTextureTriangleStripPipeline().setTexture(
    //     _image_view);
}

void VulkanImage::draw() {
    // draw the image
    const float w = (float)_width;
    const float h = (float)_height;
    const float x = 0;
    const float y = 0;

    // NOTE: openvg coordinate system is bottom, left is 0,0
    // clang-format off
    // std::array<float,16> vertices = {
	// 		x,     y,      _s[0], _t[1],  	// left, bottom
	// 		x + w, y,      _s[1], _t[1],	// right, bottom
	// 		x,     y + h,  _s[0], _t[0],	// left, top
	// 		x + w, y + h,  _s[1], _t[0] 	// right, top
	// };
    // std::array<textured_vertex_2d_t,4> vertices = {{
	// 		{x,     y,      _s[0], _t[1]},  	// left, bottom
	// 		{x + w, y,      _s[1], _t[1]},	// right, bottom
	// 		{x,     y + h,  _s[0], _t[0]},	// left, top
	// 		{x + w, y + h,  _s[1], _t[0]} 	// right, top
    // }};

    std::array<textured_vertex_2d_t,4> vertices = {{
			{x,     y,      _s[0], _t[1]},  	// left, bottom
			{x + w, y,      _s[1], _t[1]},	// right, bottom
			{x,     y + h,  _s[0], _t[0]},	// left, top
			{x + w, y + h,  _s[1], _t[0]} 	// right, top
    }};

    // std::array<textured_vertex_2d_t,6> vertices = {{
	// 		{x,     y,      _s[0], _t[1]},  // left, bottom
	// 		{x + w, y,      _s[1], _t[1]},	// right, bottom
	// 		{x,     y + h,  _s[0], _t[0]},	// left, top

    //         {x,     y + h,  _s[0], _t[0]},	// left, top
    //         {x + w, y,      _s[1], _t[1]},	// right, bottom
	// 		{x + w, y + h,  _s[1], _t[0]} 	// right, top
    // }};

    // clang-format on

    // bind texture triangle strip graphics pipeline
    getVulkanContext().getTextureTriangleStripPipeline().bind();
    // getVulkanContext().getTextureTrianglePipeline().bind();

    // copy the vertex data to the buffer
    void *data;
    vmaMapMemory(getVulkanContext().getVulkanAllocator(),
                 _vertex_buffer_allocation, &data);
    memcpy(data, vertices.data(), vertices.size() *
    sizeof(textured_vertex_2d_t));
    vmaUnmapMemory(getVulkanContext().getVulkanAllocator(),
                   _vertex_buffer_allocation);

    getVulkanContext().getTextureTriangleStripPipeline().setTexture(
        _image_view);

    // bind the vertex buffer
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(getVulkanContext().getVulkanCommandBuffer(), 0, 1,
                           &_vertex_buffer, &offset);

    // draw the image
    vkCmdDraw(getVulkanContext().getVulkanCommandBuffer(), vertices.size(), 1,
              0, 0);
}

void VulkanImage::drawSubRect(VGint ox, VGint oy, VGint w, VGint h,
                              VGbitfield paintModes) {}

void VulkanImage::drawToRect(VGint x, VGint y, VGint w, VGint h,
                             VGbitfield paintModes) {}

void VulkanImage::drawAtPoint(VGint x, VGint y, VGbitfield paintModes) {}

void VulkanImage::bind() {}

void VulkanImage::unbind() {}

} // namespace MonkVG
