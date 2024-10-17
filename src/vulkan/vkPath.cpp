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

    // get the native Vulkan context
    VulkanContext &vk_ctx = (MonkVG::VulkanContext &)getContext();

    if (paint_modes & VG_FILL_PATH) {
        buildFillIfDirty();
    }

    if (paint_modes & VG_STROKE_PATH) {
        buildStrokeIfDirty();
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