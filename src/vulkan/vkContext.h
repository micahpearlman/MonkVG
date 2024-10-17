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
#include "vkGraphicsPipeline.h"

namespace MonkVG {
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

    void setIdentity() override;
    void transform(VGfloat *t) override;
    void scale(VGfloat sx, VGfloat sy) override;
    void translate(VGfloat x, VGfloat y) override;
    void rotate(VGfloat angle) override;
    void setTransform(const VGfloat *t) override;
    void multiply(const VGfloat *t) override;

    void startBatch(IBatch *batch) override;
    void dumpBatch(IBatch *batch, void **vertices, size_t *size) override;
    void endBatch(IBatch *batch) override;

    void pushOrthoCamera(VGfloat left, VGfloat right, VGfloat bottom,
                         VGfloat top, VGfloat near, VGfloat far) override;

    void popOrthoCamera() override;

    // Vulkan specific
    bool     setVulkanContext(VkDevice logical_dev);
    VkDevice getVulkanLogicalDevice() const { return _logical_dev; }

  private:
    VkDevice _logical_dev = VK_NULL_HANDLE;

  private:
    std::unique_ptr<VulkanGraphicsPipeline> _color_pipeline   = nullptr;
    std::unique_ptr<VulkanGraphicsPipeline> _texture_pipeline = nullptr;

}; // class VulkanContext

} // namespace MonkVG

#endif // __VKCONTEXT_H__
