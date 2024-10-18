/**
 * @file vkColorPipeline.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief A color pipeline using the color.vert and color.frag shaders
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vkColorPipeline.h"

const static uint32_t color_vert[] =
#include "shaders/color.vert.h"
    ;

const static uint32_t color_frag[] =
#include "shaders/color.frag.h"
    ;

namespace MonkVG {

ColorPipeline::ColorPipeline(VulkanContext &context)
    : VulkanGraphicsPipeline<ColorPipeline_UBO>(context, color_vert,
                                                sizeof(color_vert), color_frag,
                                                sizeof(color_frag)) {

    // Define vertex input binding description
    VkVertexInputBindingDescription binding_desc = {};
    binding_desc.binding                         = 0;
    binding_desc.stride                          = sizeof(vertex_2d_t);
    binding_desc.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    // Create the graphics pipelines
    /// Color Pipeline
    // for the color pipeline the vertex type is just a 2d position
    // the color is a uniform

    std::vector<VkVertexInputAttributeDescription> vertex_input_attribs = {};
    VkVertexInputAttributeDescription              vertex_attrib        = {};
    vertex_attrib.binding                                               = 0;
    vertex_attrib.location                                              = 0;
    vertex_attrib.format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attrib.offset = offsetof(vertex_2d_t, v);
    vertex_input_attribs.push_back(vertex_attrib);

    VkPipelineVertexInputStateCreateInfo color_vertex_state = {};
    color_vertex_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    color_vertex_state.vertexBindingDescriptionCount = 1;
    color_vertex_state.pVertexBindingDescriptions    = &binding_desc;
    color_vertex_state.vertexAttributeDescriptionCount =
        vertex_input_attribs.size();
    color_vertex_state.pVertexAttributeDescriptions =
        vertex_input_attribs.data();


    _pipeline = createPipeline(color_vertex_state);
    if (_pipeline == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to create pipeline");
    }
}

ColorPipeline::~ColorPipeline() {}
} // namespace MonkVG
