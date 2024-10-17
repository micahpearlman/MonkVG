/**
 * @file vkPath.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan VG Path implementation
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __VK_PATH_H__
#define __VK_PATH_H__
#include "mkPath.h"
#include "vkPaint.h"
#include <vk_mem_alloc.h>
namespace MonkVG {
class VulkanContext;
class VulkanPath : public IPath {
  public:
    VulkanPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
               VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint,
               VGbitfield capabilities, IContext &context);
    virtual ~VulkanPath();

    bool draw(VGbitfield paintModes) override;
    void clear(VGbitfield caps) override;
    void buildFillIfDirty() override;
    void buildStrokeIfDirty() override;

private:
    VulkanContext& getVulkanContext();

  private:
    std::vector<float> _fill_vertices   = {};
    std::vector<float> _stroke_vertices = {};

    VulkanPaint *_fill_paint   = nullptr;
    VulkanPaint *_stroke_paint = nullptr;

  private:
    VkBuffer      _fill_vertex_buffer              = VK_NULL_HANDLE;
    VkBuffer      _stroke_vertex_buffer            = VK_NULL_HANDLE;
    VmaAllocation _fill_vertex_buffer_allocation   = VK_NULL_HANDLE;
    VmaAllocation _stroke_vertex_buffer_allocation = VK_NULL_HANDLE;

}; // VulkanPath

} // namespace MonkVG
#endif // __VK_PATH_H__
