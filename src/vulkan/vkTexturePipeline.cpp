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
TexturePipeline::TexturePipeline(VulkanContext &context, VkPrimitiveTopology topology)
    : VulkanGraphicsPipeline<TexturePipeline_UBO>(
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
}

TexturePipeline::~TexturePipeline() {}

} // namespace MonkVG
