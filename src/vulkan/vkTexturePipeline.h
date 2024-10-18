/**
 * @file vkTexturePipeline.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __VK_TEXTURE_PIPELINE_H__
#define __VK_TEXTURE_PIPELINE_H__

#include "vkGraphicsPipeline.h"

namespace MonkVG {

struct TexturePipeline_UBO {
    glm::mat4 u_model_view;
    glm::mat4 u_projection;
};

class TexturePipeline : public VulkanGraphicsPipeline<TexturePipeline_UBO> {
  public:
    TexturePipeline(VulkanContext &context);
    virtual ~TexturePipeline();
};

} // namespace MonkVG

#endif // __VK_TEXTURE_PIPELINE_H__
