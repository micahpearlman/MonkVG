/**
 * @file mkTessalator.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Tesselation interface class.
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __MK_TESSELATOR_H__
#define __MK_TESSELATOR_H__
#include <MonkVG/openvg.h>
#include "mkTypes.h"
#include <vector>
#include <cstdint>
namespace MonkVG {
class IPath;
class IContext;
class ITessellator {
  public:
    virtual ~ITessellator() = default;

    /**
     * @brief Tesselate the path
     * @param segments The segments of the path
     * @param coords The coordinates of the path
     * @param fill_rule The fill rule to use. Either VG_EVEN_ODD or VG_NON_ZERO.
     * @param tess_iterations The number of iterations to tesselate. The
     * higher the number the more vertices will be generated.
     * @param vertices The resulting vertices of the tessellated path
     * @param bounding_box The bounding box of the tessellated path
     */
    virtual void tessellate(const std::vector<VGubyte> &segments,
                            const std::vector<VGfloat> &coords,
                            const VGFillRule           fill_rule,
                            const uint32_t              tess_iterations,
                            std::vector<VGfloat>       &vertices,
                            bounding_box_t             &bounding_box) = 0;

    /**
     * @brief Tesselate the path
     *
     * @param path path to tessellate
     * @param tess_iterations The number of iterations to tesselate. The
     * higher the number the more vertices will be generated.
     * @param vertices The resulting vertices of the tessellated path
     * @param bounding_box The bounding box of the tessellated path.
     */
    virtual void tessellate(IPath *path, const uint32_t tess_iterations,
                            std::vector<VGfloat> &vertices,
                            bounding_box_t       &bounding_box) = 0;

    /**
     * @brief Given a path (segments and coords) build the stroke vertices.
     *
     * @param segments The segments of the path
     * @param fcoords The coordinates of the path
     * @param stroke_width The width of the stroke
     * @param tess_iterations The number of iterations to tesselate. The
     * higher the number the more vertices will be generated.
     * @param vertices The resulting vertices of the tessellated path.
     */
    void buildStroke(const std::vector<VGubyte> &segments,
                     const std::vector<VGfloat> &fcoords,
                     const float stroke_width, const uint32_t tess_iterations,
                     std::vector<vertex_2d_t> &vertices);
    void buildFatLineSegment(std::vector<vertex_2d_t> &vertices,
                             const vertex_2d_t &p0, const vertex_2d_t &p1,
                             const float stroke_width);

  protected:
    ITessellator() = default; //: _context(context) {};

    // IContext &_context;
    // IContext &getContext() { return _context; }

}; // ITesselator
} // namespace MonkVG

#endif // __MK_TESSELATOR_H__
