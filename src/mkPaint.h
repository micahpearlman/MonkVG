/*
 *  mkPaint.h
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/3/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkPaint_h__
#define __mkPaint_h__

#include "mkBaseObject.h"
#include <vector>
#include <array>

namespace MonkVG {

class IPaint : public BaseObject {
  public:
    inline BaseObject::Type getType() const { return BaseObject::kPaintType; }

    //// parameter accessors/mutators ////
    virtual VGint   getParameteri(const VGint p) const;
    virtual VGfloat getParameterf(const VGint f) const;
    virtual void    getParameterfv(const VGint p, VGfloat *fv) const;
    virtual void    setParameter(const VGint p, const VGfloat f);
    virtual void    setParameter(const VGint p, const VGint i);
    virtual void    setParameter(const VGint p, const VGfloat *fv,
                                 const VGint cnt);

    const std::array<VGfloat, 4> getPaintColor() const { return _paintColor; }

    VGPaintType  getPaintType() const { return _paintType; }
    virtual void setPaintType(VGPaintType t) { _paintType = t; }

    virtual bool isDirty() const { return _isDirty; }
    virtual void setIsDirty(bool b) { _isDirty = b; }

  protected:
    IPaint(IContext &context)
        : BaseObject(context),
          _paintType(VG_PAINT_TYPE_COLOR) // default paint type is color
          ,
          _isDirty(true) {}
    virtual ~IPaint() = default;

    bool _isDirty = true;

    VGPaintType            _paintType              = VG_PAINT_TYPE_COLOR;
    std::array<VGfloat, 4> _paintColor             = {1.0f, 1.0f, 1.0f, 1.0f};
    VGColorRampSpreadMode  _colorRampSpreadMode    = VG_COLOR_RAMP_SPREAD_PAD;
    VGboolean              _colorRampPremultiplied = VG_FALSE;
    std::array<VGfloat, 4> _paintLinearGradient    = {0.0f, 0.0f, 0.0f, 0.0f};
    std::array<VGfloat, 5> _paintRadialGradient    = {0.0f, 0.0f, 0.0f, 0.0f,
                                                      0.0f};
    std::array<VGfloat, 6> _paint2x3Gradient       = {0.0f, 0.0f, 0.0f,
                                                      0.0f, 0.0f, 0.0f};
    VGTilingMode           _patternTilingMode      = VG_TILE_FILL;

    struct Stop_t {
        VGfloat a[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    };
    std::vector<Stop_t> _colorRampStops = {};
};

} // namespace MonkVG
#endif // __mkPaint_h__