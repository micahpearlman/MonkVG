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
     * @param vertices The resulting vertices of the tessellated path
     * @param bounding_box The bounding box of the tessellated path
     */
    virtual void tessellate(const std::vector<VGubyte> &segments,
                            const std::vector<VGfloat> &coords,
                            std::vector<VGfloat>       &vertices,
                            bounding_box_t             &bounding_box) = 0;

    /**
     * @brief Tesselate the path
     *
     * @param path path to tessellate
     * @param vertices The resulting vertices of the tessellated path
     * @param bounding_box The bounding box of the tessellated path.
     */
    virtual void tessellate(IPath *path, std::vector<VGfloat> &vertices,
                            bounding_box_t &bounding_box) = 0;

  protected:
    ITessellator(IContext &context) : _context(context) {};

    IContext &_context;
    IContext &getContext() { return _context; }

}; // ITesselator
} // namespace MonkVG

#endif // __MK_TESSELATOR_H__
