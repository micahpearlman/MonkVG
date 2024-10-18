//
//  mkFont.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/29/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __mkFont_h__
#define __mkFont_h__

#include "mkBaseObject.h"
#include "mkImage.h"
#include "mkPath.h"

#include <unordered_map>
#include <memory>

namespace MonkVG {

class IFont : public BaseObject {
  public:
    inline BaseObject::Type getType() const override {
        return BaseObject::kFontType;
    }

    virtual ~IFont() = default;

    virtual void drawGlyph(VGuint index, VGfloat adj_x, VGfloat adj_y,
                           VGbitfield paintModes);

    //// parameter accessors/mutators ////
    virtual VGint   getParameteri(const VGint p) const override;
    virtual VGfloat getParameterf(const VGint f) const override;
    virtual void    getParameterfv(const VGint p, VGfloat *fv) const override;
    virtual void    setParameter(const VGint p, const VGfloat f) override;
    virtual void    setParameter(const VGint p, const VGint i) override;
    virtual void    setParameter(const VGint p, const VGfloat *fv,
                                 const VGint cnt) override;

    virtual void addGlyphImage(VGuint index_, IImage *image_,
                               VGfloat glyphOrigin_[2], VGfloat escapement_[2]);
    virtual void addGlyphPath(VGuint index_, IPath *path_,
                              VGfloat glyphOrigin_[2], VGfloat escapement_[2]);
    virtual void removeGlyph(VGuint index);

  protected:
    IFont(IContext &context) : BaseObject(context) {}

    struct Glyph {
        VGuint           index = 0;
        BaseObject::Type type; // type can be either: kPathType or kImageType
        VGfloat          glyphOrigin[2] = {0, 0};
        VGfloat          escapement[2]  = {0, 0};

        Glyph(VGuint index_, VGfloat glyphOrigin_[2], VGfloat escapement_[2])
            : index(index_), type(BaseObject::kMAXIMUM_TYPE) {
            glyphOrigin[0] = glyphOrigin_[0];
            glyphOrigin[1] = glyphOrigin_[1];

            escapement[0] = escapement_[0];
            escapement[1] = escapement_[1];
        }

        virtual ~Glyph() = default;

        virtual void draw(VGbitfield paintModes, VGfloat adj_x,
                          VGfloat adj_y) = 0;
    };

    struct GlyphImage : public Glyph {
        IImage *_image;

        GlyphImage(VGuint index_, IImage *image_, VGfloat glyphOrigin_[2],
                   VGfloat escapement_[2])
            : Glyph(index_, glyphOrigin_, escapement_), _image(image_) {
            type = BaseObject::kImageType;

            // increase the reference count of the image
            _image->incRef();
        }

        virtual ~GlyphImage() {
            // decrease the reference count of the image
            _image->decRef();
        }

        virtual void draw(VGbitfield paintModes, VGfloat adj_x, VGfloat adj_y);
    };

    struct GlyphPath : public Glyph {
        IPath *_path;

        GlyphPath(VGuint index_, IPath *path_, VGfloat glyphOrigin_[2],
                  VGfloat escapement_[2])
            : Glyph(index_, glyphOrigin_, escapement_), _path(path_) {
            type = BaseObject::kPathType;

            // increase the reference count of the path
            _path->incRef();
        }

        virtual ~GlyphPath() {
            // decrease the reference count of the path
            _path->decRef();
        }

        virtual void draw(VGbitfield paintModes, VGfloat adj_x, VGfloat adj_y) {
            _path->draw(paintModes);
        }
    };

    // glyph map, key is the glyph index. value is the glyph object.
    // note: key is usually the ascii value of the character but can be any
    // value
    std::unordered_map<VGuint, std::unique_ptr<Glyph>> _glyphs;
};

} // namespace MonkVG

#endif // __mkFont_h__
