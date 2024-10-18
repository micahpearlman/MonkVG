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
    const uint32_t *fragment_src, const size_t frag_src_sz)
    : _context(static_cast<VulkanContext &>(context)) {

    if (!compile(vertex_src, vert_src_sz, fragment_src, frag_src_sz)) {
        throw std::runtime_error("failed to compile shader");
    }

    _pipeline = createPipeline(vertex_input_state);
    if (_pipeline == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to create pipeline");
    }
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    if (_vertex_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                              _vertex_shader_module, nullptr);
    }
    if (_fragment_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                              _fragment_shader_module, nullptr);
    }
    if (_pipeline_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(getVulkanContext().getVulkanLogicalDevice(),
                                _pipeline_layout, nullptr);
    }
    if (_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(getVulkanContext().getVulkanLogicalDevice(), _pipeline,
                          nullptr);
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
    create_info.pCode    = code;

    VkShaderModule shader_module;
    if (vkCreateShaderModule(getVulkanContext().getVulkanLogicalDevice(), &create_info,
                             nullptr, &shader_module) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    return shader_module;
}

VkPipeline VulkanGraphicsPipeline::createPipeline(
    VkPipelineVertexInputStateCreateInfo &vertex_input_state) {

    // TODO: someday support triangle strip and fan topologies.
    // the tessellator can generate different topologies but for now
    // we convert the tessaletor output to triangles.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports    = &getVulkanContext().getVulkanViewport();
    viewport_state.scissorCount  = 1;
    viewport_state.pScissors     = &getVulkanContext().getVulkanScissor();

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode =
        VK_CULL_MODE_NONE; // assuming because this is UI we never want to cull
    rasterizer.frontFace =
        VK_FRONT_FACE_COUNTER_CLOCKWISE; // ?? does it matter with
                                         // VK_CULL_MODE_NONE?
    rasterizer.depthBiasEnable = VK_FALSE;

    // TODO: maybe we want to enable antialiasing???
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // NOTE: assuming we do not want depth testing
    // TODO: future we may want to support stencil buffers
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable       = VK_FALSE;
    depth_stencil.depthWriteEnable      = VK_FALSE;
    depth_stencil.depthCompareOp        = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable     = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable =
        VK_FALSE; // TODO: maybe we want to enable blending
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable   = VK_FALSE;
    color_blending.logicOp         = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments    = &color_blend_attachment;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount         = 0;
    pipeline_layout_info.pSetLayouts            = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges    = nullptr;

    if (vkCreatePipelineLayout(getVulkanContext().getVulkanLogicalDevice(),
                               &pipeline_layout_info, nullptr,
                               &_pipeline_layout) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2; // vertex and fragment
    VkPipelineShaderStageCreateInfo shader_stages[2] = {};
    shader_stages[0].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = _vertex_shader_module;
    shader_stages[0].pName  = "main";
    shader_stages[1].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage            = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module           = _fragment_shader_module;
    shader_stages[1].pName            = "main";
    pipeline_info.pStages             = shader_stages;
    pipeline_info.pVertexInputState   = &vertex_input_state;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState      = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState   = &multisampling;
    pipeline_info.pDepthStencilState  = &depth_stencil;
    pipeline_info.pColorBlendState    = &color_blending;
    pipeline_info.layout              = _pipeline_layout;
    pipeline_info.renderPass          = _context.getVulkanRenderPass();
    pipeline_info.subpass             = 0;

    if (vkCreateGraphicsPipelines(getVulkanContext().getVulkanLogicalDevice(),
                                  VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    // can destroy the shader modules now that the pipeline is created
    vkDestroyShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                          _vertex_shader_module, nullptr);
    vkDestroyShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                          _fragment_shader_module, nullptr);
    _vertex_shader_module   = VK_NULL_HANDLE;
    _fragment_shader_module = VK_NULL_HANDLE;

    return _pipeline;
}

void VulkanGraphicsPipeline::bind() {


    vkCmdBindPipeline(getVulkanContext().getVulkanCommandBuffer(),
                      VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
}

} // namespace MonkVG
