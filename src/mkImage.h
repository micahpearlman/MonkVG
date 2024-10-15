//
//  mkImage.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/28/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __mkImage_h__
#define __mkImage_h__

#include "mkBaseObject.h"
#include <array>

namespace MonkVG {

/**
 * @brief Interface for image objects.
 *
 */
class IImage : public BaseObject {
  public:

    /// @brief Get the type of the monk object type
    /// @return
    inline BaseObject::Type getType() const { return BaseObject::kImageType; }

    /// @brief Create a child image
    /// @param x
    /// @param y
    /// @param w
    /// @param h
    /// @return IImage*
    virtual IImage *createChild(VGint x, VGint y, VGint w, VGint h) = 0;

    /// @brief Draw the image
    virtual void draw() = 0;

    /// @brief Draw a sub region of the image
    /// @param ox
    /// @param oy
    /// @param w
    /// @param h
    /// @param paintModes
    virtual void drawSubRect(VGint ox, VGint oy, VGint w, VGint h,
                             VGbitfield paintModes) = 0;

    /// @brief Draw an image to a specific rectangle
    /// @param x
    /// @param y
    /// @param w
    /// @param h
    /// @param paintModes
    virtual void drawToRect(VGint x, VGint y, VGint w, VGint h,
                            VGbitfield paintModes) = 0;

    /// @brief Draw an image at a specific point
    /// @param x
    /// @param y
    /// @param paintModes
    virtual void drawAtPoint(VGint x, VGint y, VGbitfield paintModes) = 0;

    //// parameter accessors/mutators ////
    virtual VGint   getParameteri(const VGint p) const;
    virtual VGfloat getParameterf(const VGint f) const;
    virtual void    getParameterfv(const VGint p, VGfloat *fv) const;
    virtual void    setParameter(const VGint p, const VGfloat f);
    virtual void    setParameter(const VGint p, const VGint i);
    virtual void    setParameter(const VGint p, const VGfloat *fv,
                                 const VGint cnt);

    /// @brief Copies image data to a sub region of the VGImage
    /// @param data
    /// @param dataStride
    /// @param dataFormat
    /// @param x
    /// @param y
    /// @param width
    /// @param height
    virtual void setSubData(const void *data, VGint dataStride,
                            VGImageFormat dataFormat, VGint x, VGint y,
                            VGint width, VGint height) = 0;

    /// accessors
    inline VGImageFormat getFormat() const { return _format; }
    inline VGint         getWidth() const { return _width; }
    inline VGint         getHeight() const { return _height; }
    inline VGbitfield    getAllowedQuality() const { return _allowed_quality; }
    inline IImage       *getParent() const { return _parent; }

  protected:

    IImage(VGImageFormat format, VGint width, VGint height,
           VGbitfield allowedQuality, IContext &context);
    IImage(IImage &image);
    virtual ~IImage() = default;

    VGImageFormat _format          = VG_sRGBA_8888;
    VGint         _width           = 0;
    VGint         _height          = 0;
    VGbitfield    _allowed_quality = 0;

    IImage *_parent = nullptr;

    // texture coordinates
    std::array<VGfloat, 2> _s = {0, 1};
    std::array<VGfloat, 2> _t = {0, 1};
};

} // namespace MonkVG

#endif // __mkImage_h__
