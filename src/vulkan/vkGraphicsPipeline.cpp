/**
 * @file vkShader.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan shader implementation
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vkGraphicsPipeline.h"
#include "vkContext.h"

namespace MonkVG {

VulkanGraphicsPipeline::VulkanGraphicsPipeline(
    IContext &context, const uint32_t *vertex_src, const size_t vert_src_sz,
    const uint32_t *fragment_src, const size_t frag_src_sz,
    const std::vector<VkVertexInputAttributeDescription> &vertex_input_attribs)
    : _context(static_cast<VulkanContext &>(context)) {

    if (!compile(vertex_src, vert_src_sz, fragment_src, frag_src_sz)) {
        throw std::runtime_error("failed to compile shader");
    }
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    if (_vertex_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_context.getVulkanLogicalDevice(),
                              _vertex_shader_module, nullptr);
    }
    if (_fragment_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_context.getVulkanLogicalDevice(),
                              _fragment_shader_module, nullptr);
    }
}

bool VulkanGraphicsPipeline::compile(const uint32_t *vertex_src,
                                     size_t          vert_src_sz,
                                     const uint32_t *fragment_src,
                                     size_t          frag_src_sz) {
    _vertex_shader_module   = createShaderModule(vertex_src, vert_src_sz);
    _fragment_shader_module = createShaderModule(fragment_src, frag_src_sz);

    if (_vertex_shader_module == VK_NULL_HANDLE ||
        _fragment_shader_module == VK_NULL_HANDLE) {
        return false;
    }
    return true;
}

VkShaderModule VulkanGraphicsPipeline::createShaderModule(const uint32_t *code,
                                                          size_t code_size) {
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code_size;
    create_info.pCode    = (const uint32_t *)code;

    VkShaderModule shader_module;
    if (vkCreateShaderModule(_context.getVulkanLogicalDevice(), &create_info,
                             nullptr, &shader_module) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    return shader_module;
}

VkPipeline VulkanGraphicsPipeline::createPipeline(const std::vector<VkVertexInputAttributeDescription> &vertex_input_attribs) {
    // build the shader stages
    VkPipelineShaderStageCreateInfo vertex_shader_stage_info = {};
    vertex_shader_stage_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = _vertex_shader_module;
    vertex_shader_stage_info.pName  = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_stage_info = {};
    fragment_shader_stage_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = _fragment_shader_module;
    fragment_shader_stage_info.pName  = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vertex_shader_stage_info, fragment_shader_stage_info};

    // setup dynamic state
    // we will use dynamic state to for re-sizeable viewport and scissor,
    // and blend constants
    // REMEMBER THESE NEED TO BE SET AT DRAW TIME!!!!
    VkDynamicState dynamic_states[]                = {VK_DYNAMIC_STATE_VIEWPORT,
                                                      VK_DYNAMIC_STATE_SCISSOR,
                                                      VK_DYNAMIC_STATE_BLEND_CONSTANTS};
    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount =
        static_cast<uint32_t>(sizeof(dynamic_states));
    dynamic_state.pDynamicStates = dynamic_states;

    // setup vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount   = 0;
    vertex_input_info.pVertexBindingDescriptions      = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions    = nullptr;
}

} // namespace MonkVG
