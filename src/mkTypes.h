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

namespace MonkVG {
union vertex_2d_t {
    struct {
        float x, y;
    };
    float v[2];
};

struct textured_vertex_2d_t {
    vertex_2d_t vert;
    union {
        float uv[2];
        struct {
            float u, v;
        };
    };
};

struct bounding_box_t {
    float min_x = 0;
    float min_y = 0;
    float width = 0;
    float height = 0;

    bounding_box_t() = default;
    bounding_box_t(float x, float y, float w, float h)
        : min_x(x), min_y(y), width(w), height(h) {}

    // update the bounding box with the given point
    void update(float x, float y) {
        min_x = std::min(min_x, x);
        min_y = std::min(min_y, y);
        width = std::max(width, x - min_x);
        height = std::max(height, y - min_y);
    }
};

} // namespace MonkVG
#endif // __MK_TYPES_H__
