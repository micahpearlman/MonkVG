/**
 * @file vkGraphicsPipeline.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan Graphics Pipeline implementation.
 * Remember a Vulkan Graphics Pipeline is kindof like a material shader in
 * OpenGL. But also defines the fixed function state of the pipeline.
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __VK_GRAPHICS_PIPELINE_H__
#define __VK_GRAPHICS_PIPELINE_H__

#include "vkContext.h"
#include "vkPlatform.h"
#include <glm/glm.hpp>

namespace MonkVG {

template <typename VERT_UBO, typename FRAG_UBO> class VulkanGraphicsPipeline {
  public:
    VulkanGraphicsPipeline(VulkanContext &context, const uint32_t *vertex_src,
                           const size_t        vert_src_sz,
                           const uint32_t     *fragment_src,
                           const size_t        frag_src_sz,
                           VkPrimitiveTopology topology)
        : _context(static_cast<VulkanContext &>(context)), _topology(topology) {

        if (!compile(vertex_src, vert_src_sz, fragment_src, frag_src_sz)) {
            throw std::runtime_error("failed to compile shader");
        }

        // create the uniform buffer
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size               = sizeof(VERT_UBO);
        buffer_info.usage              = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(getVulkanContext().getVulkanAllocator(), &buffer_info,
                        &alloc_info, &_vert_ubo, &_uniform_buffer_allocation,
                        nullptr);

        // create the descriptor set layout
        // TODO: remove the vert ubo binding as we are using push constants
        VkDescriptorSetLayoutBinding vert_ubo_layout_binding = {};
        vert_ubo_layout_binding.binding = 0; // binding = 0 in the shader
        vert_ubo_layout_binding.descriptorType =
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vert_ubo_layout_binding.descriptorCount = 1; // 1 uniform buffer
        vert_ubo_layout_binding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding frag_sampler_layout_binding = {};
        frag_sampler_layout_binding.binding = 1; // binding = 1 in the shader
        frag_sampler_layout_binding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        frag_sampler_layout_binding.descriptorCount = 1; // 1 sampler
        frag_sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
            vert_ubo_layout_binding, frag_sampler_layout_binding};

        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings    = bindings.data();

        if (vkCreateDescriptorSetLayout(
                getVulkanContext().getVulkanLogicalDevice(), &layout_info,
                nullptr, &_descriptor_set_layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout");
        }

        VkDescriptorSetAllocateInfo desc_alloc_info = {};
        desc_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        desc_alloc_info.descriptorPool =
            getVulkanContext().getVulkanDescriptorPool();
        desc_alloc_info.descriptorSetCount = 1;
        desc_alloc_info.pSetLayouts        = &_descriptor_set_layout;

        if (vkAllocateDescriptorSets(
                getVulkanContext().getVulkanLogicalDevice(), &desc_alloc_info,
                &_descriptor_set) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor set");
        }

        VkDescriptorBufferInfo desc_buffer_info = {};
        desc_buffer_info.buffer                 = _vert_ubo;
        desc_buffer_info.offset                 = 0;
        desc_buffer_info.range                  = sizeof(VERT_UBO);

        VkWriteDescriptorSet descriptor_write = {};
        descriptor_write.sType      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet     = _descriptor_set;
        descriptor_write.dstBinding = 0;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount = 0;       // 1;
        descriptor_write.pBufferInfo     = nullptr; //&desc_buffer_info;

        vkUpdateDescriptorSets(getVulkanContext().getVulkanLogicalDevice(), 1,
                               &descriptor_write, 0, nullptr);
    }

    virtual ~VulkanGraphicsPipeline() {
        if (_vertex_shader_module != VK_NULL_HANDLE) {
            vkDestroyShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                                  _vertex_shader_module, nullptr);
        }
        if (_fragment_shader_module != VK_NULL_HANDLE) {
            vkDestroyShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                                  _fragment_shader_module, nullptr);
        }
        if (_vert_ubo != VK_NULL_HANDLE) {
            vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(), _vert_ubo,
                             _uniform_buffer_allocation);
        }
        if (_frag_ubo != VK_NULL_HANDLE) {
            vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(), _frag_ubo,
                             _uniform_buffer_allocation);
        }
        if (_descriptor_set_layout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(
                getVulkanContext().getVulkanLogicalDevice(),
                _descriptor_set_layout, nullptr);
        }
        if (_pipeline_layout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(getVulkanContext().getVulkanLogicalDevice(),
                                    _pipeline_layout, nullptr);
        }
        if (_pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(getVulkanContext().getVulkanLogicalDevice(),
                              _pipeline, nullptr);
        }
    }

    /**
     * @brief Bind the pipeline program
     *
     */
    virtual void bind() {

        vkCmdBindPipeline(getVulkanContext().getVulkanCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

        // update the uniform buffer
        // set the projection and modelview matrices
        setProjectionMatrix(getVulkanContext().getGLProjectionMatrix());
        setModelViewMatrix(getVulkanContext().getGLActiveMatrix());

        // upload the push constants to the GPU
        vkCmdPushConstants(getVulkanContext().getVulkanCommandBuffer(),
                           _pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(VERT_UBO), &_vert_ubo_data);

        // bind the descriptor set
        // NOTE: this is really only updating the texture sampler
        vkCmdBindDescriptorSets(getVulkanContext().getVulkanCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                _pipeline_layout, 0, 1, &_descriptor_set, 0,
                                nullptr);
    }

    // projection and modelview setters
    // NOTE: this assumes there is a uniform in the shader called "u_projection"
    // and "u_model_view"
    virtual void setProjectionMatrix(const glm::mat4 &matrix) {
        _vert_ubo_data.u_projection = matrix;

        // vulkan clip space has inverted Y and half Z
        // ??? This is not needed??
        // _vert_ubo_data.u_projection[1][1] *= -1;
        // _vert_ubo_data.u_projection[2][2] *= 0.5f;
    }
    virtual void setModelViewMatrix(const glm::mat4 &matrix) {
        _vert_ubo_data.u_model_view = matrix;
    }

  protected:
    VERT_UBO _vert_ubo_data = {};
    FRAG_UBO _frag_ubo_data = {};

    VulkanContext &_context;

    VulkanContext &getVulkanContext() { return _context; }

    VkShaderModule _vertex_shader_module   = VK_NULL_HANDLE;
    VkShaderModule _fragment_shader_module = VK_NULL_HANDLE;

    VkPipeline       _pipeline        = VK_NULL_HANDLE;
    VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;

    // uniform buffer/descriptor set layout
    VmaAllocation         _uniform_buffer_allocation = VK_NULL_HANDLE;
    VkBuffer              _vert_ubo                  = VK_NULL_HANDLE;
    VkBuffer              _frag_ubo                  = VK_NULL_HANDLE;
    VkDescriptorSetLayout _descriptor_set_layout     = VK_NULL_HANDLE;
    VkDescriptorSet       _descriptor_set            = VK_NULL_HANDLE;

    /**
     * @brief Create a Pipeline object
     *
     * @param vertex_input_attribs
     * @return VkPipeline
     */
    VkPipeline
    createPipeline(VkPipelineVertexInputStateCreateInfo &vertex_input_state) {
        // TODO: someday support triangle strip and fan topologies.
        // the tessellator can generate different topologies but for now
        // we convert the tessaletor output to triangles.
        VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
        input_assembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = getTopology();
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
        rasterizer.cullMode = VK_CULL_MODE_NONE; // assuming because this is UI
                                                 // we never want to cull
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

        VkPushConstantRange push_constant_range = {};
        push_constant_range.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant_range.offset              = 0;
        push_constant_range.size                = sizeof(VERT_UBO);

        VkPipelineLayoutCreateInfo pipeline_layout_info = {};
        pipeline_layout_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 1;
        pipeline_layout_info.pSetLayouts            = &_descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges    = &push_constant_range;

        if (vkCreatePipelineLayout(getVulkanContext().getVulkanLogicalDevice(),
                                   &pipeline_layout_info, nullptr,
                                   &_pipeline_layout) != VK_SUCCESS) {
            return VK_NULL_HANDLE;
        }

        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
        pipeline_info.renderPass = getVulkanContext().getVulkanRenderPass();
        pipeline_info.subpass    = 0;

        if (vkCreateGraphicsPipelines(
                getVulkanContext().getVulkanLogicalDevice(), VK_NULL_HANDLE, 1,
                &pipeline_info, nullptr, &_pipeline) != VK_SUCCESS) {
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
    /**
     * @brief Compile a shader given the vertex and fragment.
     * NOTE: NOT file path
     *
     * @param vertex_src
     * @param fragment_src
     * @return true if successful
     */
    bool compile(const uint32_t *vertex_src, size_t vert_src_sz,
                 const uint32_t *fragment_src, size_t frag_src_sz) {
        _vertex_shader_module   = createShaderModule(vertex_src, vert_src_sz);
        _fragment_shader_module = createShaderModule(fragment_src, frag_src_sz);

        if (_vertex_shader_module == VK_NULL_HANDLE ||
            _fragment_shader_module == VK_NULL_HANDLE) {
            return false;
        }
        return true;
    }

    /**
     * @brief Create a Shader Module object
     *
     * @param code
     * @param code_size
     * @return VkShaderModule
     */
    VkShaderModule createShaderModule(const uint32_t *code, size_t code_size) {
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code_size;
        create_info.pCode    = code;

        VkShaderModule shader_module;
        if (vkCreateShaderModule(getVulkanContext().getVulkanLogicalDevice(),
                                 &create_info, nullptr,
                                 &shader_module) != VK_SUCCESS) {
            return VK_NULL_HANDLE;
        }

        return shader_module;
    }

    const VkPrimitiveTopology &getTopology() const { return _topology; }

  protected:
    VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

}; // VulkanGraphicsPipeline

} // namespace MonkVG
#endif // __VK_GRAPHICS_PIPELINE_H__
