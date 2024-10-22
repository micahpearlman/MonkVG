/**
 * @file vkPaint.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan VG Paint implementation
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vkPaint.h"

namespace MonkVG {
VulkanPaint::VulkanPaint(IContext &context) : IPaint(context) {}

VulkanPaint::~VulkanPaint() {}
} // namespace MonkVG
