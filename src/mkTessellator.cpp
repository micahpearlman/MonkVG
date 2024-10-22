/**
 * @file mkTesselator.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mkTessellator.h"
#include "mkMath.h"
namespace MonkVG {

void ITessellator::buildFatLineSegment(std::vector<vertex_2d_t> &vertices,
                                       const vertex_2d_t        &p0,
                                       const vertex_2d_t        &p1,
                                       const float               stroke_width) {

    if ((p0.x == p1.x) && (p0.y == p1.y)) {
        return;
    }

    float       dx      = p1.y - p0.y;
    float       dy      = p0.x - p1.x;
    const float inv_mag = 1.0f / sqrtf(dx * dx + dy * dy);
    dx                  = dx * inv_mag;
    dy                  = dy * inv_mag;

    vertex_2d_t v0, v1, v2, v3;
    const float radius = stroke_width * 0.5f;

    v0.x = p0.x + radius * dx;
    v0.y = p0.y + radius * dy;
    vertices.push_back(v0);

    v1.x = p0.x - radius * dx;
    v1.y = p0.y - radius * dy;
    vertices.push_back(v1);

    v2.x = p1.x + radius * dx;
    v2.y = p1.y + radius * dy;
    vertices.push_back(v2);

    v3.x = p1.x - radius * dx;
    v3.y = p1.y - radius * dy;
    vertices.push_back(v3);
}

void ITessellator::buildStroke(const std::vector<VGubyte> &segments,
                               const std::vector<VGfloat> &fcoords,
                               const float                 stroke_width,
                               const uint32_t              tess_iterations,
                               std::vector<vertex_2d_t>   &vertices) {

    vertices.clear();

    auto        coords_it = fcoords.begin();
    VGbyte      segment   = VG_CLOSE_PATH;
    vertex_2d_t coords    = {0, 0};
    vertex_2d_t prev      = {0, 0};
    vertex_2d_t closeTo   = {0, 0};
    for (const auto &segment : segments) {

        // todo: deal with relative move
        bool isRelative = segment & VG_RELATIVE;
        switch (segment >> 1) {
        case (VG_CLOSE_PATH >> 1): {
            buildFatLineSegment(vertices, coords, closeTo, stroke_width);
        } break;
        case (VG_MOVE_TO >> 1): {
            prev.x = closeTo.x = coords.x = *coords_it;
            coords_it++;
            prev.y = closeTo.y = coords.y = *coords_it;
            coords_it++;

        } break;
        case (VG_LINE_TO >> 1): {
            prev     = coords;
            coords.x = *coords_it;
            coords_it++;
            coords.y = *coords_it;
            coords_it++;
            if (isRelative) {
                coords.x += prev.x;
                coords.y += prev.y;
            }

            buildFatLineSegment(vertices, prev, coords, stroke_width);

        } break;
        case (VG_HLINE_TO >> 1): {
            prev     = coords;
            coords.x = *coords_it;
            coords_it++;
            if (isRelative) {
                coords.x += prev.x;
            }

            buildFatLineSegment(vertices, prev, coords, stroke_width);
        } break;
        case (VG_VLINE_TO >> 1): {
            prev     = coords;
            coords.y = *coords_it;
            coords_it++;
            if (isRelative) {
                coords.y += prev.y;
            }

            buildFatLineSegment(vertices, prev, coords, stroke_width);

        } break;
        case (VG_SCUBIC_TO >> 1): {
            prev         = coords;
            VGfloat cp2x = *coords_it;
            coords_it++;
            VGfloat cp2y = *coords_it;
            coords_it++;
            VGfloat p3x = *coords_it;
            coords_it++;
            VGfloat p3y = *coords_it;
            coords_it++;

            if (isRelative) {
                cp2x += prev.x;
                cp2y += prev.y;
                p3x += prev.x;
                p3y += prev.y;
            }

            VGfloat cp1x = 2.0f * cp2x - p3x;
            VGfloat cp1y = 2.0f * cp2y - p3y;

            VGfloat increment = 1.0f / float(tess_iterations);
            // printf("\tcubic: ");
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                vertex_2d_t c;
                c.x = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                buildFatLineSegment(vertices, prev, c, stroke_width);
                prev = c;
            }
            coords.x = p3x;
            coords.y = p3y;

        } break;

        case (VG_QUAD_TO >> 1): // added by rhcad
        {
            prev        = coords;
            VGfloat cpx = *coords_it;
            coords_it++;
            VGfloat cpy = *coords_it;
            coords_it++;
            VGfloat px = *coords_it;
            coords_it++;
            VGfloat py = *coords_it;
            coords_it++;

            if (isRelative) {
                cpx += prev.x;
                cpy += prev.y;
                px += prev.x;
                py += prev.y;
            }

            VGfloat increment = 1.0f / float(tess_iterations);

            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                vertex_2d_t c;
                c.x = calcQuadBezier1d(coords.x, cpx, px, t);
                c.y = calcQuadBezier1d(coords.y, cpy, py, t);
                buildFatLineSegment(vertices, prev, c, stroke_width);
                prev = c;
            }
            coords.x = px;
            coords.y = py;

        } break;

        case (VG_CUBIC_TO >> 1): // todo
        {
            prev         = coords;
            VGfloat cp1x = *coords_it;
            coords_it++;
            VGfloat cp1y = *coords_it;
            coords_it++;
            VGfloat cp2x = *coords_it;
            coords_it++;
            VGfloat cp2y = *coords_it;
            coords_it++;
            VGfloat p3x = *coords_it;
            coords_it++;
            VGfloat p3y = *coords_it;
            coords_it++;

            if (isRelative) {
                cp1x += prev.x;
                cp1y += prev.y;
                cp2x += prev.x;
                cp2y += prev.y;
                p3x += prev.x;
                p3y += prev.y;
            }

            VGfloat increment = 1.0f / float(tess_iterations);

            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                vertex_2d_t c;
                c.x = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                buildFatLineSegment(vertices, prev, c, stroke_width);
                prev = c;
            }
            coords.x = p3x;
            coords.y = p3y;

        } break;
        case (VG_SCCWARC_TO >> 1):
        case (VG_SCWARC_TO >> 1):
        case (VG_LCCWARC_TO >> 1):
        case (VG_LCWARC_TO >> 1):

        {
            VGfloat rh = *coords_it;
            coords_it++;
            VGfloat rv = *coords_it;
            coords_it++;
            VGfloat rot = *coords_it;
            coords_it++;
            VGfloat cp1x = *coords_it;
            coords_it++;
            VGfloat cp1y = *coords_it;
            coords_it++;
            if (isRelative) {
                cp1x += prev.x;
                cp1y += prev.y;
            }

            // convert to Center Parameterization (see OpenVG Spec Apendix A)
            VGfloat   cx0[2];
            VGfloat   cx1[2];
            VGboolean success =
                findEllipses(rh, rv, rot, coords.x, coords.y, cp1x, cp1y,
                             &cx0[0], &cx0[1], &cx1[0], &cx1[1]);

            if (success) {
                // see:
                // http://en.wikipedia.org/wiki/Ellipse#Ellipses_in_computer_graphics
                const int steps   = float(tess_iterations);
                VGfloat   beta    = 0; // angle. todo
                VGfloat   sinbeta = sinf(beta);
                VGfloat   cosbeta = cosf(beta);

                // calculate the start and end angles
                vertex_2d_t center;
                center.x = cx0[0]; //(cx0[0] + cx1[0])*0.5f;
                center.y = cx0[1]; //(cx0[1] + cx1[1])*0.5f;
                vertex_2d_t norm[2];
                norm[0].x           = center.x - coords.x;
                norm[0].y           = center.y - coords.y;
                VGfloat inverse_len = 1.0f / sqrtf((norm[0].x * norm[0].x) +
                                                   (norm[0].y * norm[0].y));
                norm[0].x *= inverse_len;
                norm[0].y *= inverse_len;

                norm[1].x   = center.x - cp1x;
                norm[1].y   = center.y - cp1y;
                inverse_len = 1.0f / sqrtf((norm[1].x * norm[1].x) +
                                           (norm[1].y * norm[1].y));
                norm[1].x *= inverse_len;
                norm[1].y *= inverse_len;
                VGfloat startAngle = degrees(acosf(-norm[0].x));
                VGfloat endAngle   = degrees(acosf(-norm[1].x));
                VGfloat cross      = norm[0].x;
                if (cross >= 0) {
                    startAngle = 360 - startAngle;
                    endAngle   = 360 - endAngle;
                }
                if (startAngle > endAngle) {
                    VGfloat tmp = startAngle;
                    startAngle  = endAngle;
                    endAngle    = tmp;
                    startAngle  = startAngle - 90;
                    endAngle    = endAngle - 90;
                }

                prev = coords;
                for (VGfloat g = startAngle; g < endAngle + (360 / steps);
                     g += 360 / steps) {
                    vertex_2d_t c;

                    VGfloat alpha    = g * (M_PI / 180.0f);
                    VGfloat sinalpha = sinf(alpha);
                    VGfloat cosalpha = cosf(alpha);
                    c.x              = cx0[0] +
                          (rh * cosalpha * cosbeta - rv * sinalpha * sinbeta);
                    c.y = cx0[1] +
                          (rh * cosalpha * sinbeta + rv * sinalpha * cosbeta);
                    // printf( "(%f, %f)\n", c[0], c[1] );
                    buildFatLineSegment(vertices, prev, c, stroke_width);
                    prev = c;
                }
            }

            coords.x = cp1x;
            coords.y = cp1y;

        } break;

        default:
            throw std::runtime_error("Unknown segment type");
            break;
        }
    } // foreach segment
}

} // namespace MonkVG
