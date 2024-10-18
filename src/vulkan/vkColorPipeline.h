/**
 * @file vkColorPipeline.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief A color pipeline using the color.vert and color.frag shaders
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __VK_COLOR_PIPELINE_H__
#define __VK_COLOR_PIPELINE_H__

#include "vkGraphicsPipeline.h"

namespace MonkVG {
struct ColorPipeline_UBO {
    glm::mat4 u_projection;
    glm::mat4 u_model_view;
    glm::vec4 u_color;
};

class ColorPipeline : public VulkanGraphicsPipeline<ColorPipeline_UBO> {
  public:
    ColorPipeline(VulkanContext &context);
    virtual ~ColorPipeline();
};
} // namespace MonkVG
#endif // __VK_COLOR_PIPELINE_H__