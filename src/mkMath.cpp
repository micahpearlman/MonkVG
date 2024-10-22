/*
 *  mkMath.cpp
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/11/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "mkMath.h"
#include "mkContext.h"

namespace MonkVG {

void lerpStops(color_t &dst, const gradient_stop_t &stop0,
               const gradient_stop_t &stop1, const float g) {
    float den =
        std::max(0.00001f, stop1 != stop0 ? stop1[0] - stop0[0] : 1 - stop0[0]);
    for (int i = 0; i < 4; i++) {
        dst[i] =
            stop0[i + 1] + (stop1[i + 1] - stop0[i + 1]) * (g - stop0[0]) / den;
    }
}

void calcStops(const std::vector<gradient_stop_t> &stops,
               gradient_stop_t &stop0, gradient_stop_t &stop1, float g) {
    assert(g >= 0);
    assert(g <= 1);
    size_t stop_cnt = stops.size();

    // if there are no stops, set the default stops
    if (stop_cnt == 0) {
        stop0 = {0, 0, 0, 0, 1};
        stop1 = {1, 1, 1, 1, 1};
        return;
    }

    // set default stops to the first and last stops
    stop0 = stops[0];
    stop1 = stops[stop_cnt - 1];
    for (size_t i = 0; i < stop_cnt; i++) {
        const gradient_stop_t &curr = stops[i];
        if (g >= curr[0]) {
            stop0 = curr;
        } else if (g <= curr[0]) {
            stop1 = curr;
            break;
        }
    }
}

} // namespace MonkVG

using namespace MonkVG;

//// OpenVG API Implementation ////

//// Matrix Manipulation ////
VG_API_CALL void VG_API_ENTRY vgLoadIdentity(void) VG_API_EXIT {
    IContext::instance().setIdentity();
}

VG_API_CALL void VG_API_ENTRY vgLoadMatrix(const VGfloat *m) VG_API_EXIT {
    IContext::instance().setTransform(m);
}

VG_API_CALL void VG_API_ENTRY vgGetMatrix(VGfloat *m) VG_API_EXIT {
    IContext::instance().transform(m);
}

VG_API_CALL void VG_API_ENTRY vgMultMatrix(const VGfloat *m) VG_API_EXIT {
    IContext::instance().multiply(m);
}

VG_API_CALL void VG_API_ENTRY vgTranslate(VGfloat tx, VGfloat ty) VG_API_EXIT {
    IContext::instance().translate(tx, ty);
}

VG_API_CALL void VG_API_ENTRY vgScale(VGfloat sx, VGfloat sy) VG_API_EXIT {
    IContext::instance().scale(sx, sy);
}

// VG_API_CALL void VG_API_ENTRY vgShear(VGfloat shx, VGfloat shy) VG_API_EXIT;

VG_API_CALL void VG_API_ENTRY vgRotate(VGfloat angle) VG_API_EXIT {
    IContext::instance().rotate(angle);
}
