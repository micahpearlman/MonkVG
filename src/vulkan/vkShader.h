/**
 * @file vkShader.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan shader implementation
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __VK_SHADER_H__
#define __VK_SHADER_H__

#include "mkContext.h"
#include "vkPlatform.h"
#include <glm/glm.hpp>

namespace MonkVG {
class VulkanContext;
class VulkanShader {
  public:
    VulkanShader(IContext &context);
    virtual ~VulkanShader();

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
     * @brief Bind the shader program
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

    VkShaderModule createShaderModule(const uint32_t *code, size_t code_size);

}; // VulkanShader

} // namespace MonkVG
#endif // __VK_SHADER_H__
