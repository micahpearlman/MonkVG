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
#include "vkTexturePipeline.h"
#include <glm/gtc/matrix_transform.hpp>

const static uint32_t color_vert[] =
#include "shaders/color.vert.h"
    ;

const static uint32_t color_frag[] =
#include "shaders/color.frag.h"
    ;

// DEBUG: if you uncomment the following code you should see a triangle
// rendered. const static uint32_t color_vert[] = #include "shaders/test.vert.h"
//     ;

// const static uint32_t color_frag[] =
// #include "shaders/test.frag.h"
//     ;

namespace MonkVG {

ColorPipeline::ColorPipeline(VulkanContext      &context,
                             VkPrimitiveTopology topology)
    : VulkanGraphicsPipeline<ColorPipeline_VertUBO, ColorPipeline_FragUBO>(context, color_vert,
                                                sizeof(color_vert), color_frag,
                                                sizeof(color_frag), topology) {

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

    // set some sane defaults
    _vert_ubo_data.u_color      = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    _vert_ubo_data.u_projection = glm::ortho(0.0f, (float)context.getWidth(), 0.0f,
                                        (float)context.getHeight());
    // _vert_ubo_data.u_model_view = glm::mat4(1.0f);
    // _vert_ubo_data.u_model_view = glm::translate(_vert_ubo_data.u_model_view,
    //                                         glm::vec3(1.0f, 1.0f, 0.0f));
}

ColorPipeline::~ColorPipeline() {}
} // namespace MonkVG
