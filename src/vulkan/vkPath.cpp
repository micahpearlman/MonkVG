/**
 * @file vkPath.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Vulkan VG Path implementation
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vkPath.h"
#include "vkContext.h"
#include "vkColorPipeline.h"
#include "vkTexturePipeline.h"
namespace MonkVG {
VulkanPath::VulkanPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
                       VGfloat bias, VGint segmentCapacityHint,
                       VGint coordCapacityHint, VGbitfield capabilities,
                       IContext &context)
    : IPath(pathFormat, datatype, scale, bias, segmentCapacityHint,
            coordCapacityHint, capabilities, context) {}

VulkanPath::~VulkanPath() {}

VulkanContext &VulkanPath::getVulkanContext() {
    return static_cast<VulkanContext &>(getContext());
}

bool VulkanPath::draw(VGbitfield paint_modes) {
    // if there are no paint modes then do nothing
    if (paint_modes == 0) {
        return false;
    }

    if (paint_modes & VG_FILL_PATH) {
        buildFillIfDirty();
        // figure out the appropriate pipeline and bind it
        if (_fill_paint) {
            if (_fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
                // get the color pipeline
                getVulkanContext().getColorPipeline().bind();
            } else if (_fill_paint->getPaintType() ==
                       VG_PAINT_TYPE_LINEAR_GRADIENT) {
                // get the linear gradient pipeline
                getVulkanContext().getTexturePipeline().bind();
            } else if (_fill_paint->getPaintType() ==
                       VG_PAINT_TYPE_RADIAL_GRADIENT) {
                // get the radial gradient pipeline
                getVulkanContext().getTexturePipeline().bind();
            } else if (_fill_paint->getPaintType() == VG_PAINT_TYPE_PATTERN) {
                // get the pattern pipeline
                getVulkanContext().getTexturePipeline().bind();
            } else {
                // error
                // HACK: we are just going to bind the color pipeline
                getVulkanContext().getColorPipeline().bind();
            }
        } else {
            // DELETE THIS
            // HACK: we are just going to bind the color pipeline
            getVulkanContext().getColorPipeline().bind();
        }

        // bind the vertex buffer
        VkBuffer     vertex_buffers[] = {_fill_vertex_buffer};
        VkDeviceSize offsets[]        = {0};
        vkCmdBindVertexBuffers(getVulkanContext().getVulkanCommandBuffer(), 0,
                               1, vertex_buffers, offsets);

        // draw the fill
        vkCmdDraw(getVulkanContext().getVulkanCommandBuffer(),
                  _fill_vertices.size() / 2, 1, 0, 0);
    }

    if (paint_modes & VG_STROKE_PATH) {
        buildStrokeIfDirty();

        // TODO: implement stroke
    }

    return true;
}

void VulkanPath::clear(VGbitfield caps) {}

void VulkanPath::buildFillIfDirty() {
    IPaint *current_fill_paint = getContext().getFillPaint();
    if (current_fill_paint != _fill_paint) {
        _fill_paint = (VulkanPaint *)current_fill_paint;
        setFillDirty(true);
    }
    // only build the fill if dirty or we are in batch build mode
    if (getIsFillDirty() || getContext().currentBatch()) {
        // tessellate the path
        getContext().getTessellator().tessellate(_segments, _fcoords,
                                                 _fill_vertices, _bounds);

        // create the vertex buffer
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size               = _fill_vertices.size() * sizeof(float);
        buffer_info.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(getVulkanContext().getVulkanAllocator(), &buffer_info,
                        &alloc_info, &_fill_vertex_buffer,
                        &_fill_vertex_buffer_allocation, nullptr);
    }
    setFillDirty(false);
}

void VulkanPath::buildStrokeIfDirty() {}

} // namespace MonkVG