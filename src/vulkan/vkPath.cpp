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

bool VulkanPath::draw(VGbitfield paintModes) { return false; }

void VulkanPath::clear(VGbitfield caps) {}

void VulkanPath::buildFillIfDirty() {
    // only build the fill if dirty or we are in batch build mode
    if (_is_fill_dirty || getContext().currentBatch()) {
        // tessellate the path
        getContext().getTessellator().tessellate(_segments, _fcoords,
                                                 _fill_vertices, _bounds);
    }
    _is_fill_dirty = false;
}

void VulkanPath::buildStrokeIfDirty() {}

} // namespace MonkVG