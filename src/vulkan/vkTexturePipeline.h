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

struct TexturePipeline_VertUBO {
    glm::mat4 u_model_view;
    glm::mat4 u_projection;
};

struct TexturePipeline_FragUBO {
    glm::vec4 u_color;
};

class TexturePipeline : public VulkanGraphicsPipeline<TexturePipeline_VertUBO,
                                                      TexturePipeline_FragUBO> {
  public:
    TexturePipeline(VulkanContext &context, VkPrimitiveTopology topology);
    ~TexturePipeline();

    void bind() override;

    // this pipeline uses a texture so we need to pass in the image info
    // VkDescriptorImageInfo *getDescriptorImageInfo() override {
    //     return &_descriptor_image_info;
    // }

    /**
     * @brief Set the Texture object. Will update the descriptor set
     * with the new texture.
     *
     * @param texture
     */
    void setTexture(VkImageView texture);

    /**
     * @brief Get the Texture object
     *
     * @return VkImageView
     */
    VkImageView getTexture() const { return _texture; }

  protected:
    VkImageView _texture = VK_NULL_HANDLE;
    VkSampler   _sampler = VK_NULL_HANDLE;
};

} // namespace MonkVG

#endif // __VK_TEXTURE_PIPELINE_H__
