/*
 *  mkPath.h
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkPath_h__
#define __mkPath_h__

#include "mkBaseObject.h"
#include "mkMath.h"
#include "mkTessellator.h"
#include <vector>

namespace MonkVG {

class IPath : public BaseObject {
  public:
    inline BaseObject::Type getType() const override {
        return BaseObject::kPathType;
    }

    virtual ~IPath() = default;

    /// @brief Draw the path.  See: vgDrawPath
    /// @param paintModes VGbitfield of VG_FILL_PATH and/or VG_STROKE_PATH
    /// @return bool true if successful
    virtual bool draw(VGbitfield paintModes) = 0;

    /// @brief Clear the path.  See: vgClearPath
    /// @param caps VGbitfield of VG_PATH_CAPABILITY
    virtual void clear(VGbitfield caps);

    /// @brief Build the fill if the path is dirty
    virtual void buildFillIfDirty() = 0;

    /// @brief Build the stroke if the path is dirty
    virtual void buildStrokeIfDirty() = 0;

    inline VGint getFormat() const { return _format; }
    inline void  setFormat(const VGint format) { _format = format; }

    inline VGPathDatatype getDataType() const { return _datatype; }
    inline void           setDataType(const VGPathDatatype d) { _datatype = d; }

    inline VGfloat getScale() const { return _scale; }
    inline void    setScale(const VGfloat scale) { _scale = scale; }

    inline VGfloat getBias() const { return _bias; }
    inline void    setBias(const VGfloat bias) { _bias = bias; }

    inline VGint getNumSegments() const { return _num_segments; }
    inline void  setNumSegments(const VGint num_segments) {
        _num_segments = num_segments;
    }

    inline VGint getNumCoords() const { return _num_coords; }
    inline void  setNumCoords(const VGint num_coords) {
        _num_coords = num_coords;
    }

    inline VGbitfield getCapabilities() const { return _capabilities; }
    inline void       setCapabilities(const VGbitfield c) { _capabilities = c; }

    inline bool getIsFillDirty() const { return _is_fill_dirty; }
    inline void setFillDirty(bool b) { _is_fill_dirty = b; }
    inline bool getIsStrokeDirty() const { return _is_stroke_dirty; }
    inline void setStrokeDirty(bool b) { _is_stroke_dirty = b; }

    // bounds
    inline VGfloat               getMinX() const { return _bounds.min_x; }
    inline VGfloat               getMinY() const { return _bounds.min_y; }
    inline VGfloat               getWidth() const { return _bounds.width; }
    inline VGfloat               getHeight() const { return _bounds.height; }
    inline const bounding_box_t &getBounds() const { return _bounds; }
    inline void setBounds(const bounding_box_t &bounds) { _bounds = bounds; }

    //// OpenVG parameter accessors/mutators ////
    virtual VGint   getParameteri(const VGint p) const override;
    virtual VGfloat getParameterf(const VGint format) const override;
    virtual void    getParameterfv(const VGint p, VGfloat *fv) const override;
    virtual void    setParameter(const VGint p, const VGfloat format) override;
    virtual void    setParameter(const VGint p, const VGint i) override;
    virtual void    setParameter(const VGint p, const VGfloat *fv,
                                 const VGint cnt) override;

    //// internal data manipulators ////

    /// @brief Append data to the path
    /// @param numSegments the number of segments
    /// @param pathSegments the segments
    /// @param pathData the path data
    void appendData(const VGint numSegments, const VGubyte *pathSegments,
                    const void *pathData);

    /// @brief Get the number of coordinates for a segment type.
    /// different segments have different number of coordinates.
    /// @param segment
    /// @return
    uint32_t segmentToNumCoordinates(VGPathSegment segment);

    /// @brief Copy the path data from src to this path
    /// with a transformation matrix.
    /// @param src the source path
    /// @param transform the transformation matrix
    void copy(const IPath &src, const Matrix33 &transform);

  protected:
    /// @brief Constructor
    /// @param format VG_PATH_FORMAT
    /// @param datatype VG_PATH_DATATYPE
    /// @param scale VG_PATH_SCALE
    /// @param bias VG_PATH_BIAS
    /// @param num_segments VG_PATH_NUM_SEGMENTS
    /// @param num_coords VG_PATH_NUM_COORDS
    /// @param capabilities VG_PATH_CAPABILITY
    /// @param context the MonkVG context
    explicit IPath(VGint format, VGPathDatatype datatype, VGfloat scale,
                   VGfloat bias, VGint num_segments, VGint num_coords,
                   VGbitfield capabilities, IContext &context)
        : BaseObject(context),
          _format(format),
          _datatype(datatype),
          _scale(scale),
          _bias(bias),
          _num_segments(num_segments),
          _num_coords(num_coords),
          _capabilities(capabilities),
          _is_fill_dirty(true),
          _is_stroke_dirty(true) {
        switch (_datatype) {
        case VG_PATH_DATATYPE_F:
            _fcoords = std::vector<float>(_num_coords);
            break;
        default:
            // error
            throw std::runtime_error(
                "Unsupported path data type. Currently only VG_PATH_DATATYPE_F "
                "is supported.");
            break;
        }
        _bounds.min_x = _bounds.min_y = VG_MAX_FLOAT;
        _bounds.width = _bounds.height = -VG_MAX_FLOAT;
    }

  protected:
    VGint          _format;       // VG_PATH_FORMAT
    VGPathDatatype _datatype;     // VG_PATH_DATATYPE
    VGfloat        _scale;        // VG_PATH_SCALE
    VGfloat        _bias;         // VG_PATH_BIAS
    VGint          _num_segments; // VG_PATH_NUM_SEGMENTS
    VGint          _num_coords;   // VG_PATH_NUM_COORDS
    VGbitfield     _capabilities;

    // data
    std::vector<VGubyte> _segments;
    std::vector<VGfloat> _fcoords;
    bool                 _is_fill_dirty;
    bool                 _is_stroke_dirty;

    bounding_box_t _bounds;
};
} // namespace MonkVG
#endif //__mkPath_h__