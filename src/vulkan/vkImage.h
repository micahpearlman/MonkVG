/**
 * @file vkImage.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief OpenVG image implementation
 * @version 0.1
 * @date 2024-10-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __VKIMAGE_H__
#define __VKIMAGE_H__
#include "mkImage.h"
#include "vkPlatform.h"
#include <vk_mem_alloc.h>
namespace MonkVG {
class VulkanContext;
class VulkanImage : public IImage {
  public:
    VulkanImage(VGImageFormat format, VGint width, VGint height,
                VGbitfield allowedQuality, IContext &context);
    VulkanImage(VulkanImage &other);
    virtual ~VulkanImage();

    virtual IImage *createChild(VGint x, VGint y, VGint w, VGint h);

    // drawing
    virtual void draw();
    virtual void drawSubRect(VGint ox, VGint oy, VGint w, VGint h,
                             VGbitfield paintModes);
    virtual void drawToRect(VGint x, VGint y, VGint w, VGint h,
                            VGbitfield paintModes);
    virtual void drawAtPoint(VGint x, VGint y, VGbitfield paintModes);

    virtual void setSubData(const void *data, VGint dataStride,
                            VGImageFormat dataFormat, VGint x, VGint y,
                            VGint width, VGint height);

    void bind();
    void unbind();

    VulkanContext &getVulkanContext() { return (VulkanContext &)getContext(); }

    const uint32_t getPixelSizeBytes() const { return _pixel_size_bytes; }

  protected:
    VkImage       _image            = VK_NULL_HANDLE;
    VkImageView   _image_view       = VK_NULL_HANDLE;
    VmaAllocation _image_allocation = VK_NULL_HANDLE;
    uint32_t      _pixel_size_bytes = 0;

    // vertex buffer
    VkBuffer      _vertex_buffer            = VK_NULL_HANDLE;
    VmaAllocation _vertex_buffer_allocation = VK_NULL_HANDLE;
};
} // namespace MonkVG
#endif // __VKIMAGE_H__
