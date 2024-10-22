/**
 * @file vkPaint.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan VG Paint implementation
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __VK_PAINT_H__
#define __VK_PAINT_H__
#include "mkPaint.h"
namespace MonkVG {
class VulkanPaint : public IPaint {
  public:
    VulkanPaint(IContext &context);
    virtual ~VulkanPaint();
}; // VulkanPaint
} // namespace MonkVG
#endif // __VK_PAINT_H__
