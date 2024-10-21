/**
 * @file mkTypes.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief MonkVG types
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __MK_TYPES_H__
#define __MK_TYPES_H__

#include <algorithm>
#include <MonkVG/openvg.h>

namespace MonkVG {

/**
 * @brief A "plain" 2d position vertex.
 *
 */
union vertex_2d_t {
    struct {
        float x, y;
    };
    float v[2];
};

/**
 * @brief A 2d position + texture coordinate vertex.
 *
 */
struct textured_vertex_2d_t {
    vertex_2d_t vert;
    union {
        float uv[2];
        struct {
            float u, v;
        };
    };
};

/**
 * @brief Bounding box with min x, min y, width, and height.
 * NOTE: OpenVG origin is lower left corner.
 *
 */
struct bounding_box_t {
    float min_x  = 0;
    float min_y  = 0;
    float width  = 0;
    float height = 0;

    bounding_box_t() = default;
    bounding_box_t(float x, float y, float w, float h)
        : min_x(x), min_y(y), width(w), height(h) {}

    /**
     * @brief Add a point that the bounding box should contain and
     * expand the bounding box if necessary.
     *
     * @param x
     * @param y
     */
    void update(float x, float y) {
        min_x  = std::min(min_x, x);
        min_y  = std::min(min_y, y);
        width  = std::max(width, x - min_x);
        height = std::max(height, y - min_y);
    }
};

/**
 * @brief a gradient stop is a position (index 0) and a color (index 1-4)
 * 
 */
using gradient_stop_t = std::array<VGfloat, 5>;

/**
 * @brief a color is a 4 element array of floats
 * 
 */
using color_t = std::array<VGfloat, 4>;

} // namespace MonkVG
#endif // __MK_TYPES_H__
