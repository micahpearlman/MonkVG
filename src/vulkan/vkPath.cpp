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

VulkanPath::~VulkanPath() {
    if (_fill_vertex_buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(),
                         _fill_vertex_buffer, _fill_vertex_buffer_allocation);
    }
    if (_stroke_vertex_buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(),
                         _stroke_vertex_buffer,
                         _stroke_vertex_buffer_allocation);
    }
}

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
                getVulkanContext().getColorTrianglePipeline().setColor(
                    _fill_paint->getPaintColor());
                getVulkanContext().getColorTrianglePipeline().bind();
            } else if (_fill_paint->getPaintType() ==
                       VG_PAINT_TYPE_LINEAR_GRADIENT) {
                // get the linear gradient pipeline
                getVulkanContext().getTextureTrianglePipeline().bind();
            } else if (_fill_paint->getPaintType() ==
                       VG_PAINT_TYPE_RADIAL_GRADIENT) {
                // get the radial gradient pipeline
                getVulkanContext().getTextureTrianglePipeline().bind();
            } else if (_fill_paint->getPaintType() == VG_PAINT_TYPE_PATTERN) {
                // get the pattern pipeline
                getVulkanContext().getTextureTrianglePipeline().bind();
            } else {
                // error
                // HACK: we are just going to bind the color pipeline
                getVulkanContext().getColorTrianglePipeline().bind();
            }
        } else {
            // DELETE THIS
            // HACK: we are just going to bind the color pipeline
            getVulkanContext().getColorTrianglePipeline().bind();
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
        if (_stroke_paint) {
            if (_stroke_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
                // get the color pipeline
                getVulkanContext().getColorTriangleStripPipeline().setColor(
                    _stroke_paint->getPaintColor());
                getVulkanContext().getColorTriangleStripPipeline().bind();
            } else if (_stroke_paint->getPaintType() ==
                       VG_PAINT_TYPE_LINEAR_GRADIENT) {
                // get the linear gradient pipeline
                getVulkanContext().getTextureTriangleStripPipeline().bind();
            } else if (_stroke_paint->getPaintType() ==
                       VG_PAINT_TYPE_RADIAL_GRADIENT) {
                // get the radial gradient pipeline
                getVulkanContext().getTextureTriangleStripPipeline().bind();
            } else if (_stroke_paint->getPaintType() == VG_PAINT_TYPE_PATTERN) {
                // get the pattern pipeline
                getVulkanContext().getTextureTriangleStripPipeline().bind();
            } else {
                // error
                // HACK: we are just going to bind the color pipeline
                getVulkanContext().getColorTriangleStripPipeline().bind();
            }
        } else {
            // DELETE THIS
            // HACK: we are just going to bind the color pipeline
            getVulkanContext().getColorTriangleStripPipeline().bind();
        }

        // bind the vertex buffer
        VkBuffer     vertex_buffers[] = {_stroke_vertex_buffer};
        VkDeviceSize offsets[]        = {0};
        vkCmdBindVertexBuffers(getVulkanContext().getVulkanCommandBuffer(), 0,
                               1, vertex_buffers, offsets);

        // draw the stroke
        vkCmdDraw(getVulkanContext().getVulkanCommandBuffer(),
                  _stroke_vertices.size(), 1, 0, 0);
    }

    return true;
}

void VulkanPath::clear(VGbitfield caps) {}

void VulkanPath::buildFillIfDirty() {
    IPaint *current_fill_paint = getContext().getFillPaint();

    // TODO: do not necessarily need to rebuild the fill if the paint only
    // changed color
    if (current_fill_paint != _fill_paint) {
        _fill_paint = (VulkanPaint *)current_fill_paint;
        setFillDirty(true);
    }
    // only build the fill if dirty or we are in batch build mode
    if (getIsFillDirty() || getContext().currentBatch()) {
        _fill_vertices.clear();
        // tessellate the path
        getContext().getTessellator().tessellate(
            _segments, _fcoords, getContext().getFillRule(),
            getContext().getTessellationIterations(), _fill_vertices, _bounds);            

        // DEBUG: uncomment to draw a triangle
        // also may want to comment out the MVP matrix multiplication in the
        // vertex shader
        // _fill_vertices.clear(); _fill_vertices = {0.0, -0.5,
        // 0.5, 0.5, -0.5, 0.5};
        if (_fill_vertex_buffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(),
                             _fill_vertex_buffer,
                             _fill_vertex_buffer_allocation);
            _fill_vertex_buffer              = VK_NULL_HANDLE;
            _fill_vertex_buffer_allocation   = VK_NULL_HANDLE;
        }

        // create the vertex buffer
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size               = _fill_vertices.size() * sizeof(float);
        buffer_info.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

        if (vmaCreateBuffer(getVulkanContext().getVulkanAllocator(),
                            &buffer_info, &alloc_info, &_fill_vertex_buffer,
                            &_fill_vertex_buffer_allocation,
                            nullptr) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer");
        }

        // copy the vertex data to the buffer
        void *data;
        vmaMapMemory(getVulkanContext().getVulkanAllocator(),
                     _fill_vertex_buffer_allocation, &data);
        memcpy(data, _fill_vertices.data(),
               _fill_vertices.size() * sizeof(float));
        vmaUnmapMemory(getVulkanContext().getVulkanAllocator(),
                       _fill_vertex_buffer_allocation);
    }
    setFillDirty(false);
}

void VulkanPath::buildStrokeIfDirty() {
    IPaint *current_stroke_paint = getContext().getStrokePaint();
    if (current_stroke_paint != _stroke_paint) {
        _stroke_paint = (VulkanPaint *)current_stroke_paint;
        setStrokeDirty(true);
    }
    // only build the stroke if dirty or we are in batch build mode
    if (getIsStrokeDirty() || getContext().currentBatch()) {
        _stroke_vertices.clear();
        getContext().getTessellator().buildStroke(
            _segments, _fcoords, getContext().getStrokeLineWidth(),
            getContext().getTessellationIterations(), _stroke_vertices);

        if (_stroke_vertex_buffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(getVulkanContext().getVulkanAllocator(),
                             _stroke_vertex_buffer,
                             _stroke_vertex_buffer_allocation);
            _stroke_vertex_buffer            = VK_NULL_HANDLE;
            _stroke_vertex_buffer_allocation = VK_NULL_HANDLE;
        }

        // create the vertex buffer
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size               = _stroke_vertices.size() * sizeof(vertex_2d_t);
        buffer_info.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

        if (vmaCreateBuffer(getVulkanContext().getVulkanAllocator(),
                            &buffer_info, &alloc_info, &_stroke_vertex_buffer,
                            &_stroke_vertex_buffer_allocation,
                            nullptr) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer");
        }

        // copy the vertex data to the buffer
        void *data;
        vmaMapMemory(getVulkanContext().getVulkanAllocator(),
                     _stroke_vertex_buffer_allocation, &data);
        memcpy(data, _stroke_vertices.data(),
                _stroke_vertices.size() * sizeof(vertex_2d_t));
        vmaUnmapMemory(getVulkanContext().getVulkanAllocator(),
                       _stroke_vertex_buffer_allocation);
    }
    setStrokeDirty(false);
}

} // namespace MonkVG