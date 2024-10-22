/**
 * @file mkGradient.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Generate gradient images for use in paints.
 * @version 0.1
 * @date 2024-10-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __MK_GRADIENT_H__
#define __MK_GRADIENT_H__

#include "mkTypes.h"
#include <MonkVG/openvg.h>
#include <array>
#include <vector>

namespace MonkVG {

VGImage buildLinearGradientImage(const std::array<VGfloat, 4> &linear_gradient,
                                 const std::vector<gradient_stop_t> &stops,
                                 const VGColorRampSpreadMode spread_mode,
                                 const VGfloat               path_width,
                                 const VGfloat               path_height);
VGImage buildRadialGradientImage(const std::array<VGfloat, 5> &radial_gradient,
                                 const std::vector<gradient_stop_t> &stops,
                                 const VGColorRampSpreadMode spread_mode,
                                 const VGfloat               path_width,
                                 const VGfloat               path_height);
VGImage buildLinear2x3GradientImage(const std::array<VGfloat, 6> &gradient_2x3,
                                    const std::vector<gradient_stop_t> &stops,
                                    const VGColorRampSpreadMode spread_mode,
                                    const VGfloat               path_width,
                                    const VGfloat               path_height);
VGImage buildRadial2x3GradientImage(const std::array<VGfloat, 6> &gradient_2x3,
                                    const std::vector<gradient_stop_t> &stops,
                                    const VGColorRampSpreadMode spread_mode,
                                    const VGfloat               path_width,
                                    const VGfloat               path_height);

} // namespace MonkVG

#endif // __MK_GRADIENT_H__