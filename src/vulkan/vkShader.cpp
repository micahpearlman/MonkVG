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
#include "vkShader.h"
#include "vkContext.h"

namespace MonkVG {

VulkanShader::VulkanShader(IContext &context)
    : _context(static_cast<VulkanContext &>(context)) {}

VulkanShader::~VulkanShader() {
    if (_vertex_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_context.getVulkanLogicalDevice(),
                              _vertex_shader_module, nullptr);
    }
    if (_fragment_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_context.getVulkanLogicalDevice(),
                              _fragment_shader_module, nullptr);
    }
}

bool VulkanShader::compile(const uint32_t *vertex_src, size_t vert_src_sz,
                           const uint32_t *fragment_src, size_t frag_src_sz) {
    _vertex_shader_module   = createShaderModule(vertex_src, vert_src_sz);
    _fragment_shader_module = createShaderModule(fragment_src, frag_src_sz);

    if (_vertex_shader_module == VK_NULL_HANDLE ||
        _fragment_shader_module == VK_NULL_HANDLE) {
        return false;
    }

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

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages    = shader_stages;

    if (vkCreateGraphicsPipelines(_context.getVulkanLogicalDevice(),
                                  VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        return false;
    }

    return true;
}

VkShaderModule VulkanShader::createShaderModule(const uint32_t *code,
                                                size_t          code_size) {
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code_size * 4;
    create_info.pCode    = (const uint32_t *)code;

    VkShaderModule shader_module;
    if (vkCreateShaderModule(_context.getVulkanLogicalDevice(), &create_info,
                             nullptr, &shader_module) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    return shader_module;
}

} // namespace MonkVG
