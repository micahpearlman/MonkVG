/**
 * @file vkContext.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan context management implementation.
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __VKCONTEXT_H__
#define __VKCONTEXT_H__

#include "mkContext.h"
#include "vkPlatform.h"
#include <vk_mem_alloc.h>

namespace MonkVG {
class ColorPipeline;
class TexturePipeline;
class VulkanContext : public IContext {
  public:
    VulkanContext();
    ~VulkanContext() = default;

    // singleton instance
    bool Initialize() override;
    bool Terminate() override;

    //// factories ////
    IPath  *createPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
                       VGfloat bias, VGint segmentCapacityHint,
                       VGint      coordCapacityHint,
                       VGbitfield capabilities) override;
    void    destroyPath(IPath *path) override;
    IPaint *createPaint() override;
    void    destroyPaint(IPaint *paint) override;
    IImage *createImage(VGImageFormat format, VGint width, VGint height,
                        VGbitfield allowedQuality) override;
    void    destroyImage(IImage *image) override;
    IBatch *createBatch() override;
    void    destroyBatch(IBatch *batch) override;
    IFont  *createFont() override;
    void    destroyFont(IFont *font) override;

    //// platform specific execution of stroke and fill ////
    void stroke() override;
    void fill() override;

    //// platform specific execution of Masking and Clearing ////
    void clear(VGint x, VGint y, VGint width, VGint height) override;
    void flush() override;
    void finish() override;

    void resize() override;

    void startBatch(IBatch *batch) override;
    void dumpBatch(IBatch *batch, void **vertices, size_t *size) override;
    void endBatch(IBatch *batch) override;


    // Vulkan specific
    /**
     * @brief Setup the Vulkan context for MonkVG from the application Vulkan
     * context.
     *
     * @param instance Vulkan instance
     * @param physical_device Vulkan physical device
     * @param logical_dev Vulkan logical device
     * @param render_pass Vulkan render pass
     * @param command_buffer Vulkan command buffer
     * @return true
     * @return false
     */
    bool setVulkanContext(VkInstance instance, VkPhysicalDevice physical_device,
                          VkDevice logical_dev, VkRenderPass render_pass,
                          VkCommandBuffer  command_buffer,
                          VkDescriptorPool descriptor_pool);

    /**
     * @brief Get the Vulkan Instance object
     *
     * @return VkInstance
     */
    VkInstance getVulkanInstance() const { return _instance; }

    /**
     * @brief Get the Vulkan Physical Device object
     *
     * @return VkPhysicalDevice
     */
    VkPhysicalDevice getVulkanPhysicalDevice() const { return _phys_dev; }

    /**
     * @brief Get the Vulkan Logical Device object
     *
     * @return VkDevice
     */
    VkDevice getVulkanLogicalDevice() const { return _logical_dev; }

    /**
     * @brief Get the Vulkan Render Pass object
     *
     *
     * @return VkRenderPass
     */
    VkRenderPass getVulkanRenderPass() const { return _render_pass; }

    /**
     * @brief Get the Vulkan viewport.
     *
     */
    const VkViewport &getVulkanViewport() const { return _viewport; }

    /**
     * @brief Get the Vulkan scissor
     *
     */
    const VkRect2D &getVulkanScissor() const { return _scissor; }

    /**
     * @brief Get the Vulkan allocator
     *
     */
    VmaAllocator getVulkanAllocator() const { return _allocator; }

    /**
     * @brief Get the Vulkan command buffer
     *
     */
    VkCommandBuffer getVulkanCommandBuffer() const { return _command_buffer; }

    /**
     * @brief Get the Vulkan descriptor pool
     *
     */
    VkDescriptorPool getVulkanDescriptorPool() const {
        return _descriptor_pool;
    }

    /**
     * @brief Get the color pipeline
     *
     */
    ColorPipeline &getColorPipeline() const { return *_color_pipeline; }

    /**
     * @brief Get the texture pipeline
     *
     */
    TexturePipeline &getTexturePipeline() const { return *_texture_pipeline; }

  private: /// Vulkan state passed in by the application
    VkInstance       _instance        = VK_NULL_HANDLE;
    VkPhysicalDevice _phys_dev        = VK_NULL_HANDLE;
    VkDevice         _logical_dev     = VK_NULL_HANDLE;
    VkRenderPass     _render_pass     = VK_NULL_HANDLE;
    VkCommandBuffer  _command_buffer  = VK_NULL_HANDLE;
    VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;

    bool _own_descriptor_pool = false;

  private: /// Internal Vulkan state
    // Graphics Pipelines
    std::unique_ptr<ColorPipeline>   _color_pipeline   = nullptr;
    std::unique_ptr<TexturePipeline> _texture_pipeline = nullptr;

    // Viewport & Scissor
    VkViewport _viewport = {};
    VkRect2D   _scissor  = {};

    // allocator
    VmaAllocator _allocator = VK_NULL_HANDLE;

}; // class VulkanContext

} // namespace MonkVG

#endif // __VKCONTEXT_H__
