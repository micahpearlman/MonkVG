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
#include <vector>
using namespace std;

namespace MonkVG {

class IPath : public BaseObject {
  public:
	
    virtual bool draw(VGbitfield paintModes) = 0;

    virtual void clear(VGbitfield caps);

    inline BaseObject::Type getType() const override { return BaseObject::kPathType; }

    inline VGint getFormat() const { return _format; }
    inline void  setFormat(const VGint f) { _format = f; }

    inline VGPathDatatype getDataType() const { return _datatype; }
    inline void           setDataType(const VGPathDatatype d) { _datatype = d; }

    inline VGfloat getScale() const { return _scale; }
    inline void    setScale(const VGfloat s) { _scale = s; }

    inline VGfloat getBias() const { return _bias; }
    inline void    setBias(const VGfloat b) { _bias = b; }

    inline VGint getNumSegments() const { return _numSegments; }
    inline void  setNumSegments(const VGint ns) { _numSegments = ns; }

    inline VGint getNumCoords() const { return _numCoords; }
    inline void  setNumCoords(const VGint nc) { _numCoords = nc; }

    inline VGbitfield getCapabilities() const { return _capabilities; }
    inline void       setCapabilities(const VGbitfield c) { _capabilities = c; }

    inline bool getIsDirty() { return _is_fill_dirty; }
    inline void setIsDirty(bool b) {
        _is_fill_dirty   = b;
        _isStrokeDirty = b;
    }

    // bounds
    inline VGfloat getMinX() { return _minX; }
    inline VGfloat getMinY() { return _minY; }
    inline VGfloat getWidth() { return _width; }
    inline VGfloat getHeight() { return _height; }

    //// OpenVG parameter accessors/mutators ////
    virtual VGint   getParameteri(const VGint p) const override;
    virtual VGfloat getParameterf(const VGint f) const override;
    virtual void    getParameterfv(const VGint p, VGfloat *fv) const override;
    virtual void    setParameter(const VGint p, const VGfloat f) override;
    virtual void    setParameter(const VGint p, const VGint i) override;
    virtual void    setParameter(const VGint p, const VGfloat *fv,
                                 const VGint cnt) override;

    //// internal data manipulators ////
    void    appendData(const VGint numSegments, const VGubyte *pathSegments,
                       const void *pathData);
    int32_t segmentToNumCoordinates(VGPathSegment segment);
    void    copy(const IPath &src, const Matrix33 &transform);
    virtual void buildFillIfDirty() = 0;

  protected:
    explicit IPath(VGint f, VGPathDatatype dt, VGfloat s, VGfloat b, VGint ns,
                   VGint nc, VGbitfield cap)
        : _format(f),
          _datatype(dt),
          _scale(s),
          _bias(b),
          _numSegments(ns),
          _numCoords(nc),
          _capabilities(cap),
          _is_fill_dirty(true),
          _isStrokeDirty(true),
          _minX(VG_MAX_FLOAT),
          _minY(VG_MAX_FLOAT),
          _width(-VG_MAX_FLOAT),
          _height(-VG_MAX_FLOAT) {
        switch (_datatype) {
        case VG_PATH_DATATYPE_F:
            _fcoords = new vector<float>(_numCoords);
            break;
        default:
            // error
            break;
        }
    }

    virtual ~IPath() {
        switch (_datatype) {
        case VG_PATH_DATATYPE_F:
            _fcoords->clear();
            delete _fcoords;
            _fcoords = 0;
            break;
        default:
            // error
            break;
        }
    }

  protected:
    VGint          _format;      // VG_PATH_FORMAT
    VGPathDatatype _datatype;    // VG_PATH_DATATYPE
    VGfloat        _scale;       // VG_PATH_SCALE
    VGfloat        _bias;        // VG_PATH_BIAS
    VGint          _numSegments; // VG_PATH_NUM_SEGMENTS
    VGint          _numCoords;   // VG_PATH_NUM_COORDS
    VGbitfield     _capabilities;

    // data
    vector<VGubyte>  _segments;
    vector<VGfloat> *_fcoords;
    bool             _is_fill_dirty;
    bool             _isStrokeDirty;

    // bounds
    VGfloat _minX;
    VGfloat _minY;
    VGfloat _height;
    VGfloat _width;
};
} // namespace MonkVG
#endif //__mkPath_h__