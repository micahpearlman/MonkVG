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

#include "mkContext.h"
#include "vkPlatform.h"
#include <glm/glm.hpp>

namespace MonkVG {
class VulkanContext;
class VulkanGraphicsPipeline {
  public:
    VulkanGraphicsPipeline(IContext &context, const uint32_t *vertex_src,
                           const size_t    vert_src_sz,
                           const uint32_t *fragment_src,
                           const size_t    frag_src_sz,
                           const std::vector<VkVertexInputAttributeDescription>
                               &vertex_input_attribs);
    virtual ~VulkanGraphicsPipeline();

    /**
     * @brief Bind the pipeline program
     *
     */
    void bind();

    /**
     * @brief Unbind the shader program
     *
     */
    void unbind();

    // uniform setters
    void setUniform1i(const char *name, int value);
    void setUniform1f(const char *name, float value);
    void setUniform2f(const char *name, float x, float y);
    void setUniform3f(const char *name, float x, float y, float z);
    void setUniform4f(const char *name, float x, float y, float z, float w);
    void setUniformMatrix4fv(const char *name, float *matrix);

    // uniform getters
    // GLuint getUniformLocation(const char *name);

    // projection and modelview setters
    // NOTE: this assumes there is a uniform in the shader called "u_projection"
    // and "u_model_view"
    void setProjectionMatrix(const glm::mat4 &matrix);
    void setModelViewMatrix(const glm::mat4 &matrix);

    // color setter
    // NOTE: this assumes there is a uniform in the shader called "u_color"
    void setColor(const glm::vec4 &color);

  private:
    VulkanContext &_context;

    VkShaderModule _vertex_shader_module   = VK_NULL_HANDLE;
    VkShaderModule _fragment_shader_module = VK_NULL_HANDLE;

    VkPipeline _pipeline = VK_NULL_HANDLE;

    /**
     * @brief Create a Pipeline object
     *
     * @param vertex_input_attribs
     * @return VkPipeline
     */
    VkPipeline
    createPipeline(const std::vector<VkVertexInputAttributeDescription>
                       &vertex_input_attribs);

    /**
     * @brief Compile a shader given the vertex and fragment.
     * NOTE: NOT file path
     *
     * @param vertex_src
     * @param fragment_src
     * @return true if successful
     */
    bool compile(const uint32_t *vertex_src, size_t vert_src_sz,
                 const uint32_t *fragment_src, size_t frag_src_sz);

    /**
     * @brief Create a Shader Module object
     *
     * @param code
     * @param code_size
     * @return VkShaderModule
     */
    VkShaderModule createShaderModule(const uint32_t *code, size_t code_size);

}; // VulkanGraphicsPipeline

} // namespace MonkVG
#endif // __VK_GRAPHICS_PIPELINE_H__
