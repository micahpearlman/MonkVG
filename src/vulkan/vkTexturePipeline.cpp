/**
 * @file vkTexturePipeline.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vkTexturePipeline.h"
#include "vkContext.h"
#include "mkTypes.h"

const static uint32_t texture_vert[] =
#include "shaders/texture.vert.h"
    ;

const static uint32_t texture_frag[] =
#include "shaders/texture.frag.h"
    ;

namespace MonkVG {
TexturePipeline::TexturePipeline(VulkanContext      &context,
                                 VkPrimitiveTopology topology)
    : VulkanGraphicsPipeline<TexturePipeline_VertUBO, TexturePipeline_FragUBO>(
          context, texture_vert, sizeof(texture_vert), texture_frag,
          sizeof(texture_frag), topology) {

    // Define vertex input binding description
    VkVertexInputBindingDescription binding_desc = {};
    binding_desc.binding                         = 0;
    binding_desc.stride                          = sizeof(textured_vertex_2d_t);
    binding_desc.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    // Create the graphics pipelines
    /// Texture Pipeline
    // for the texture pipeline the vertex type is a 2d position and a 2d uv
    // the texture is a uniform

    std::vector<VkVertexInputAttributeDescription> vertex_input_attribs = {};
    VkVertexInputAttributeDescription              vertex_attrib        = {};
    vertex_attrib.binding                                               = 0;
    vertex_attrib.location                                              = 0;
    vertex_attrib.format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attrib.offset = offsetof(textured_vertex_2d_t, v);
    vertex_input_attribs.push_back(vertex_attrib);

    vertex_attrib.binding  = 0;
    vertex_attrib.location = 1;
    vertex_attrib.format   = VK_FORMAT_R32G32_SFLOAT;
    vertex_attrib.offset   = offsetof(textured_vertex_2d_t, uv);
    vertex_input_attribs.push_back(vertex_attrib);

    VkPipelineVertexInputStateCreateInfo texture_vertex_state = {};
    texture_vertex_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    texture_vertex_state.vertexBindingDescriptionCount = 1;
    texture_vertex_state.pVertexBindingDescriptions    = &binding_desc;
    texture_vertex_state.vertexAttributeDescriptionCount =
        vertex_input_attribs.size();
    texture_vertex_state.pVertexAttributeDescriptions =
        vertex_input_attribs.data();

    _pipeline = createPipeline(texture_vertex_state);
    if (_pipeline == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to create pipeline");
    }

    // create the sampler
    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter           = VK_FILTER_LINEAR;
    sampler_info.minFilter           = VK_FILTER_LINEAR;
    sampler_info.addressModeU        = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV        = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW        = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.anisotropyEnable    = VK_FALSE;
    sampler_info.maxAnisotropy       = 1.0f;
    sampler_info.borderColor         = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable           = VK_FALSE;
    sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias              = 0.0f;
    sampler_info.minLod                  = 0.0f;
    sampler_info.maxLod                  = 0.0f;

    if (vkCreateSampler(getVulkanContext().getVulkanLogicalDevice(),
                        &sampler_info, nullptr, &_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler");
    }
}

void TexturePipeline::setTexture(VkImageView texture) {
    _texture = texture;

    // update the descriptor set with the new texture
    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView             = _texture;
    image_info.sampler               = _sampler;

    VkWriteDescriptorSet descriptor_write = {};
    descriptor_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet          = _descriptor_set;
    descriptor_write.dstBinding      = 1; // binding = 1 in the shader
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pImageInfo      = &image_info;

    vkUpdateDescriptorSets(getVulkanContext().getVulkanLogicalDevice(), 1,
                           &descriptor_write, 0, nullptr);
}

void TexturePipeline::bind() {
    VulkanGraphicsPipeline<TexturePipeline_VertUBO, TexturePipeline_FragUBO>::bind();

    // bind the texture
    // vkCmdBindDescriptorSets(_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         _pipeline_layout, 0, 1, &_descriptor_set, 0,
    //                         nullptr);
}

TexturePipeline::~TexturePipeline() {
    if (_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(getVulkanContext().getVulkanLogicalDevice(), _sampler,
                         nullptr);
    }
}

} // namespace MonkVG
