/**
 * @file mkGradient.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mkGradient.h"
#include "mkMath.h"
#include <cassert>

namespace MonkVG {
VGImage buildLinearGradientImage(const std::array<VGfloat, 4> &linear_gradient,
                                 const std::vector<gradient_stop_t> &stops,
                                 const VGColorRampSpreadMode spread_mode,
                                 const VGfloat               path_width,
                                 const VGfloat               path_height) {
    // generated image sizes
    const int     width = 64, height = 64;
    unsigned int *image =
        (unsigned int *)malloc(width * height * sizeof(unsigned int));
    //	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
    //	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
    //	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,

    const int stop_cnt = (int)stops.size();

    //	from OpenVG specification PDF
    //
    //			dx(x - x0) + dy((y - y0)
    // g(x,y) = ------------------------
    //				dx^2 + dy^2
    // where dx = x1 - x0, dy = y1 - y0
    //
    VGfloat p0[2] = {(linear_gradient[0] / path_width) * width,
                     (linear_gradient[1] / path_height) * height};
    VGfloat p1[2] = {(linear_gradient[2] / path_width) * width,
                     (linear_gradient[3] / path_height) * height};

    float dx          = p1[0] - p0[0];
    float dy          = p1[1] - p0[1];
    float denominator = (dx * dx) + (dy * dy);
    assert(denominator != 0);
    float inv_denominator = 1.0f / denominator;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float numerator = dx * (x - p0[0]) + dy * (y - p0[1]);
            float g         = numerator * inv_denominator;

            // color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
            // where c0 = stop color 0, c1 = stop color 1
            // where x0 = stop offset 0, x1 = stop offset 1
            color_t         finalcolor;
            gradient_stop_t stop0;
            gradient_stop_t stop1;

            if (spread_mode == VG_COLOR_RAMP_SPREAD_PAD) {
                if (g < 0) {
                    stop0 = stops[0];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else if (g > 1) {
                    stop0 = stops[stop_cnt - 1];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else {
                    calcStops(stops, stop0, stop1, g);
                    lerpStops(finalcolor, stop0, stop1, g);
                }
            } else {
                int w = int(fabsf(g));

                if (spread_mode == VG_COLOR_RAMP_SPREAD_REPEAT) {
                    if (g < 0) {
                        g = 1 - (fabs(g) - w);
                    } else {
                        g = g - w;
                    }
                } else if (spread_mode == VG_COLOR_RAMP_SPREAD_REFLECT) {
                    if (g < 0) {
                        if (w % 2 == 0) { // even
                            g = (fabsf(g) - w);
                        } else { // odd
                            g = (1 - (fabsf(g) - w));
                        }

                    } else {
                        if (w % 2 == 0) { // even
                            g = g - w;
                        } else { // odd
                            g = 1 - (g - w);
                        }
                    }
                }

                g = std::clamp(g, 0.0f, 1.0f);
                calcStops(stops, stop0, stop1, g);
                lerpStops(finalcolor, stop0, stop1, g);
            }

            unsigned int color = (uint32_t(finalcolor[3] * 255) << 24) |
                                 (uint32_t(finalcolor[2] * 255) << 16) |
                                 (uint32_t(finalcolor[1] * 255) << 8) |
                                 (uint32_t(finalcolor[0] * 255) << 0);

            image[(y * width) + x] = color;
        }
    }

    // create openvg image
    VGImage gradient_image = vgCreateImage(VG_sRGBA_8888, width, height, 0);

    vgImageSubData(gradient_image, image, -1, VG_sRGBA_8888, 0, 0, width,
                   height);

    free(image);

    return gradient_image;
}

VGImage buildRadialGradientImage(const std::array<VGfloat, 5> &radial_gradient,
                                 const std::vector<gradient_stop_t> &stops,
                                 const VGColorRampSpreadMode spread_mode,
                                 const VGfloat               path_width,
                                 const VGfloat               path_height) {
    // generated image sizes
    const int     width = 64, height = 64;
    unsigned int *image =
        (unsigned int *)malloc(width * height * sizeof(unsigned int));
    //	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
    //	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
    //	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,

    const int stop_cnt = (int)stops.size();

    //	from OpenVG specification PDF
    //
    // VG_PAINT_RADIAL_GRADIENT. { cx, cy, fx, fy, r }.
    //
    //					(dx * fx' + dy * fy') + sqrt( r^2 *
    //(dx^2 + dy^2) - (dx * fy' - dy fx') ^ 2 ) 		g(x,y)	=
    //-----------------------------------------------------------------------------
    //												r^2
    //- (fx'^2 + fy'^2)
    // where:
    //		fx' = fx - cx, fy' = fy - cy
    //		dx = x - fx, dy = y - fy
    //

    // normalize the focal point
    float fxn = ((radial_gradient[2]) / path_width) * width;
    float fyn = ((radial_gradient[3]) / path_height) * height;
    float fxp = fxn - (((radial_gradient[0]) / path_width) * width);
    float fyp = fyn - (((radial_gradient[1]) / path_height) * height);

    // ??? normalizing radius on the path width but it could be either or???
    float rn = (radial_gradient[4] / path_width) * width;

    float denominator = (rn * rn) - (fxp * fxp + fyp * fyp);
    float inv_den     = 1.0f / denominator;

    for (int x = 0; x < width; x++) {
        float dx = x - fxn;
        for (int y = 0; y < height; y++) {
            float dy = y - fyn;

            float numerator = (dx * fxp + dy * fyp);
            float df        = dx * fyp - dy * fxp;
            numerator += sqrtf((rn * rn) * (dx * dx + dy * dy) - (df * df));
            float g = numerator * inv_den;

            // color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
            // where c0 = stop color 0, c1 = stop color 1
            // where x0 = stop offset 0, x1 = stop offset 1
            color_t         finalcolor;
            gradient_stop_t stop0;
            gradient_stop_t stop1;

            if (spread_mode == VG_COLOR_RAMP_SPREAD_PAD) {
                if (g < 0) {
                    stop0 = stops[0];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else if (g > 1) {
                    stop0 = stops[stop_cnt - 1];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else {
                    calcStops(stops, stop0, stop1, g);
                    lerpStops(finalcolor, stop0, stop1, g);
                }
            } else {
                int w = int(fabsf(g));

                if (spread_mode == VG_COLOR_RAMP_SPREAD_REPEAT) {
                    if (g < 0) {
                        g = 1 - (fabs(g) - w);
                    } else {
                        g = g - w;
                    }
                } else if (spread_mode == VG_COLOR_RAMP_SPREAD_REFLECT) {
                    if (g < 0) {
                        if (w % 2 == 0) { // even
                            g = (fabsf(g) - w);
                        } else { // odd
                            g = (1 - (fabsf(g) - w));
                        }

                    } else {
                        if (w % 2 == 0) { // even
                            g = g - w;
                        } else { // odd
                            g = 1 - (g - w);
                        }
                    }
                }

                // clamp
                g = std::clamp(g, 0.0f, 1.0f);

                calcStops(stops, stop0, stop1, g);
                lerpStops(finalcolor, stop0, stop1, g);
            }

            unsigned int color = (uint32_t(finalcolor[3] * 255) << 24) |
                                 (uint32_t(finalcolor[2] * 255) << 16) |
                                 (uint32_t(finalcolor[1] * 255) << 8) |
                                 (uint32_t(finalcolor[0] * 255) << 0);

            image[(y * width) + x] = color;
        }
    }

    // create openvg image
    VGImage gradient_image = vgCreateImage(VG_sRGBA_8888, width, height, 0);

    vgImageSubData(gradient_image, image, -1, VG_sRGBA_8888, 0, 0, width,
                   height);

    free(image);

    return gradient_image;
}

VGImage buildLinear2x3GradientImage(const std::array<VGfloat, 6> &gradient_2x3,
                                    const std::vector<gradient_stop_t> &stops,
                                    const VGColorRampSpreadMode spread_mode,
                                    const VGfloat               path_width,
                                    const VGfloat               path_height) {
    // generated image sizes
    const int     width = 64, height = 64;
    unsigned int *image =
        (unsigned int *)malloc(width * height * sizeof(unsigned int));
    //	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
    //	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
    //	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,

    const int stop_cnt = (int)stops.size();

    //	from OpenVG specification PDF
    //
    //			dx(x - x0) + dy((y - y0)
    // g(x,y) = ------------------------
    //				dx^2 + dy^2
    // where dx = x1 - x0, dy = y1 - y0
    //

    /*
     Setup x & y values baed on gradient size:
        x|y = (scale * 32768 * 0.05)
     Two transformations:
        1. Flash transform, based on matrix:
            x' = x + y * r1 + tx
            y' = y + x * r0 + ty
        2. Conversion to OpenVG space:
            x'' = (x' / path_width) * width
            y'' = (y' / path_height) * height
     */

    // initialize gradient space
    float   gradient_sz = 32768 * 0.05; // ??? what is this value ???
    VGfloat p1[2]       = {gradient_sz / 2, gradient_sz / 2};
    VGfloat p0[2]       = {-p1[0], p1[1]};

    // flash trasformation based on matrix values
    VGfloat p0p[2] = {gradient_2x3[0] * p0[0] + p0[1] * gradient_2x3[5],
                      gradient_2x3[1] * p0[1] + p0[0] * gradient_2x3[4]};
    VGfloat p1p[2] = {gradient_2x3[0] * p1[0] + p1[1] * gradient_2x3[5],
                      gradient_2x3[1] * p1[1] + p1[0] * gradient_2x3[4]};

    // convert to the shapes space
    p0[0] = (p0p[0] + path_width / 2) / path_width * width;
    p0[1] = (p0p[1] + path_height / 2) / path_height * height;
    p1[0] = (p1p[0] + path_width / 2) / path_width * width;
    p1[1] = (p1p[1] + path_height / 2) / path_height * height;

    // float gradientWidth = _paint2x3Gradient[0] * 32768 * 0.05; // scale
    // gradients float delta = (path_width-gradientWidth) / path_width; VGfloat
    // p0[2] = { (delta/2) * width, 0}; VGfloat p1[2] = { p0[0] +
    // ((gradientWidth / path_width) * width), 0};

    float dx          = p1[0] - p0[0];
    float dy          = p1[1] - p0[1];
    float denominator = (dx * dx) + (dy * dy);
    // todo: assert denominator != 0

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float numerator = dx * (x - p0[0]) + dy * (y - p0[1]);
            float g         = numerator / denominator;

            // color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
            // where c0 = stop color 0, c1 = stop color 1
            // where x0 = stop offset 0, x1 = stop offset 1
            color_t         finalcolor;
            gradient_stop_t stop0;
            gradient_stop_t stop1;

            if (spread_mode == VG_COLOR_RAMP_SPREAD_PAD) {
                if (g < 0) {
                    stop0 = stops[0];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else if (g < stops[0][0]) { // if the first color is after 0
                                              // fill with the first color
                    stop0 = stops[0];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }
                } else if (g > 1) {
                    stop0 = stops[stop_cnt - 1];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else if (g > stops[stop_cnt - 1]
                                    [0]) { // if the last color is before 1.0
                                           // fill with the last color
                    stop0 = stops[stop_cnt - 1];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }
                } else {
                    calcStops(stops, stop0, stop1, g);
                    lerpStops(finalcolor, stop0, stop1, g);
                }
            } else {
                int w = int(fabsf(g));

                if (spread_mode == VG_COLOR_RAMP_SPREAD_REPEAT) {
                    if (g < 0) {
                        g = 1 - (fabs(g) - w);
                    } else {
                        g = g - w;
                    }
                } else if (spread_mode == VG_COLOR_RAMP_SPREAD_REFLECT) {
                    if (g < 0) {
                        if (w % 2 == 0) { // even
                            g = (fabsf(g) - w);
                        } else { // odd
                            g = (1 - (fabsf(g) - w));
                        }

                    } else {
                        if (w % 2 == 0) { // even
                            g = g - w;
                        } else { // odd
                            g = 1 - (g - w);
                        }
                    }
                }

                // clamp
                g = std::clamp(g, 0.0f, 1.0f);
                calcStops(stops, stop0, stop1, g);
                lerpStops(finalcolor, stop0, stop1, g);
            }

            unsigned int color = (uint32_t(finalcolor[3] * 255) << 24) |
                                 (uint32_t(finalcolor[2] * 255) << 16) |
                                 (uint32_t(finalcolor[1] * 255) << 8) |
                                 (uint32_t(finalcolor[0] * 255) << 0);

            image[(y * width) + x] = color;
        }
    }

    // create openvg image
    VGImage gradient_image = vgCreateImage(VG_sRGBA_8888, width, height, 0);

    vgImageSubData(gradient_image, image, -1, VG_sRGBA_8888, 0, 0, width,
                   height);

    free(image);

    return gradient_image;
}

VGImage buildRadial2x3GradientImage(const std::array<VGfloat, 6> &gradient_2x3,
                                    const std::vector<gradient_stop_t> &stops,
                                    const VGColorRampSpreadMode spread_mode,
                                    const VGfloat               path_width,
                                    const VGfloat               path_height) {
    // generated image sizes
    const int     width = 64, height = 64;
    unsigned int *image =
        (unsigned int *)malloc(width * height * sizeof(unsigned int));
    //	VG_COLOR_RAMP_SPREAD_PAD                    = 0x1C00,
    //	VG_COLOR_RAMP_SPREAD_REPEAT                 = 0x1C01,
    //	VG_COLOR_RAMP_SPREAD_REFLECT                = 0x1C02,

    const int stop_cnt = (int)stops.size();

    //	from OpenVG specification PDF
    //
    // VG_PAINT_RADIAL_GRADIENT. { cx, cy, fx, fy, r }.
    //
    //					(dx * fx' + dy * fy') + sqrt( r^2 *
    //(dx^2 + dy^2) - (dx * fy' - dy fx') ^ 2 ) 		g(x,y)	=
    //-----------------------------------------------------------------------------
    //												r^2
    //- (fx'^2 + fy'^2)
    // where:
    //		fx' = fx - cx, fy' = fy - cy
    //		dx = x - fx, dy = y - fy
    //

    // normalize the focal point
    float fxn = width / 2;
    float fyn = height / 2;
    float fxp = fxn - (width / 2);
    float fyp = fyn - (height / 2);

    // ??? normalizing radius on the path width but it could be either or???
    float rn = width / 2;

    float denominator = (rn * rn) - (fxp * fxp + fyp * fyp);
    float inv_den     = 1.0f / denominator;

    for (int x = 0; x < width; x++) {
        float dx = x - fxn;
        for (int y = 0; y < height; y++) {
            float dy = y - fyn;

            float numerator = (dx * fxp + dy * fyp);
            float df        = dx * fyp - dy * fxp;
            numerator += sqrtf((rn * rn) * (dx * dx + dy * dy) - (df * df));
            float g = numerator * inv_den;

            // color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
            // where c0 = stop color 0, c1 = stop color 1
            // where x0 = stop offset 0, x1 = stop offset 1
            color_t         finalcolor;
            gradient_stop_t stop0;
            gradient_stop_t stop1;

            if (spread_mode == VG_COLOR_RAMP_SPREAD_PAD) {
                if (g < 0) {
                    stop0 = stops[0];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }

                } else if (g < stops[0][0]) { // if the first color is after 0
                                              // fill with the first color
                    stop0 = stops[0];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }
                } else if (g > 1) {
                    stop0 = stops[stop_cnt - 1];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }
                } else if (g > stops[stop_cnt - 1]
                                    [0]) { // if the last color is before 1.0
                                           // fill with the last color
                    stop0 = stops[stop_cnt - 1];
                    for (int i = 0; i < 4; i++) {
                        finalcolor[i] = stop0[i + 1];
                    }
                } else {
                    calcStops(stops, stop0, stop1, g);
                    lerpStops(finalcolor, stop0, stop1, g);
                }
            } else {
                int w = int(fabsf(g));

                if (spread_mode == VG_COLOR_RAMP_SPREAD_REPEAT) {
                    if (g < 0) {
                        g = 1 - (fabs(g) - w);
                    } else {
                        g = g - w;
                    }
                } else if (spread_mode == VG_COLOR_RAMP_SPREAD_REFLECT) {
                    if (g < 0) {
                        if (w % 2 == 0) { // even
                            g = (fabsf(g) - w);
                        } else { // odd
                            g = (1 - (fabsf(g) - w));
                        }

                    } else {
                        if (w % 2 == 0) { // even
                            g = g - w;
                        } else { // odd
                            g = 1 - (g - w);
                        }
                    }
                }

                // clamp
                g = std::clamp(g, 0.0f, 1.0f);
                calcStops(stops, stop0, stop1, g);
                lerpStops(finalcolor, stop0, stop1, g);
            }

            unsigned int color = (uint32_t(finalcolor[3] * 255) << 24) |
                                 (uint32_t(finalcolor[2] * 255) << 16) |
                                 (uint32_t(finalcolor[1] * 255) << 8) |
                                 (uint32_t(finalcolor[0] * 255) << 0);

            image[(y * width) + x] = color;
        }
    }

    // create openvg image
    VGImage gradient_image = vgCreateImage(VG_sRGBA_8888, width, height, 0);

    vgImageSubData(gradient_image, image, -1, VG_sRGBA_8888, 0, 0, width,
                   height);

    free(image);

    return gradient_image;
}

} // namespace MonkVG
