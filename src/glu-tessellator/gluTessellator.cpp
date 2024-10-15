/**
 * @file gluTesselator.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief GLU Tesselator implementation
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "gluTessellator.h"
#include "mkContext.h"
#include <iostream>

namespace MonkVG {

GLUTessellator::GLUTessellator(IContext &context) : ITessellator(context) {
    // BUGBUG: wanted to avoid resizing as we pass a pointer to the data
    // in gluTessVertex
    // _tess_verts.reserve(1024);
}

void GLUTessellator::tessellate(const std::vector<VGubyte> &segments,
                                const std::vector<VGfloat> &fcoords,
                                std::vector<VGfloat>       &vertices,
                                bounding_box_t             &bounding_box) {

    _out_vertices = &vertices;
    _out_bounding_box = &bounding_box;

    // create the tesselator
    _glu_tessellator = gluNewTess();

    // set the callback functions
    gluTessCallback(_glu_tessellator, GLU_TESS_BEGIN_DATA,
                    (GLvoid(APIENTRY *)()) & GLUTessellator::tessBegin);
    gluTessCallback(_glu_tessellator, GLU_TESS_END_DATA,
                    (GLvoid(APIENTRY *)()) & GLUTessellator::tessEnd);
    gluTessCallback(_glu_tessellator, GLU_TESS_VERTEX_DATA,
                    (GLvoid(APIENTRY *)()) & GLUTessellator::tessVertex);
    gluTessCallback(_glu_tessellator, GLU_TESS_COMBINE_DATA,
                    (GLvoid(APIENTRY *)()) & GLUTessellator::tessCombine);
    gluTessCallback(_glu_tessellator, GLU_TESS_ERROR,
                    (GLvoid(APIENTRY *)()) & GLUTessellator::tessError);

    if (getContext().getFillRule() == VG_EVEN_ODD) {
        gluTessProperty(_glu_tessellator, GLU_TESS_WINDING_RULE,
                        GLU_TESS_WINDING_ODD);
    } else if (getContext().getFillRule() == VG_NON_ZERO) {
        gluTessProperty(_glu_tessellator, GLU_TESS_WINDING_RULE,
                        GLU_TESS_WINDING_NONZERO);
    }
    gluTessProperty(_glu_tessellator, GLU_TESS_TOLERANCE,
                    0.5f); // HARDWIRED: TODO: make this a parameter

    gluTessBeginPolygon(_glu_tessellator, this);
    auto   coordsIter = fcoords.begin();
    VGbyte segment    = VG_CLOSE_PATH;
    v3_t   coords(0, 0, 0);
    v3_t   prev(0, 0, 0);
    int    num_contours = 0;

    for (const auto &segment : segments) {

        // todo: deal with relative move
        bool isRelative = segment & VG_RELATIVE;
        switch (segment >> 1) {
        case (VG_CLOSE_PATH >> 1): {
            if (num_contours) {
                gluTessEndContour(_glu_tessellator);
                num_contours--;
            }

        } break;
        case (VG_MOVE_TO >> 1): {
            if (num_contours) {
                gluTessEndContour(_glu_tessellator);
                num_contours--;
            }

            gluTessBeginContour(_glu_tessellator);
            num_contours++;
            coords.x = *coordsIter;
            coordsIter++;
            coords.y = *coordsIter;
            coordsIter++;

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_glu_tessellator, l, l);

        } break;
        case (VG_LINE_TO >> 1): {
            prev     = coords;
            coords.x = *coordsIter;
            coordsIter++;
            coords.y = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.x += prev.x;
                coords.y += prev.y;
            }

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_glu_tessellator, l, l);
        } break;
        case (VG_HLINE_TO >> 1): {
            prev     = coords;
            coords.x = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.x += prev.x;
            }

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_glu_tessellator, l, l);
        } break;
        case (VG_VLINE_TO >> 1): {
            prev     = coords;
            coords.y = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.y += prev.y;
            }

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_glu_tessellator, l, l);
        } break;
        case (VG_SCUBIC_TO >> 1): {
            prev         = coords;
            VGfloat cp2x = *coordsIter;
            coordsIter++;
            VGfloat cp2y = *coordsIter;
            coordsIter++;
            VGfloat p3x = *coordsIter;
            coordsIter++;
            VGfloat p3y = *coordsIter;
            coordsIter++;

            if (isRelative) {
                cp2x += prev.x;
                cp2y += prev.y;
                p3x += prev.x;
                p3y += prev.y;
            }

            VGfloat cp1x = 2.0f * cp2x - p3x;
            VGfloat cp1y = 2.0f * cp2y - p3y;

            VGfloat increment = 1.0f / getContext().getTessellationIterations();
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v3_t c;
                c.x         = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y         = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                c.z         = 0;
                GLdouble *l = addTessVertex(c);
                gluTessVertex(_glu_tessellator, l, l);
            }
            coords.x = p3x;
            coords.y = p3y;

        } break;
        case (VG_CUBIC_TO >> 1): {
            prev         = coords;
            VGfloat cp1x = *coordsIter;
            coordsIter++;
            VGfloat cp1y = *coordsIter;
            coordsIter++;
            VGfloat cp2x = *coordsIter;
            coordsIter++;
            VGfloat cp2y = *coordsIter;
            coordsIter++;
            VGfloat p3x = *coordsIter;
            coordsIter++;
            VGfloat p3y = *coordsIter;
            coordsIter++;

            if (isRelative) {
                cp1x += prev.x;
                cp1y += prev.y;
                cp2x += prev.x;
                cp2y += prev.y;
                p3x += prev.x;
                p3y += prev.y;
            }

            VGfloat increment = 1.0f / getContext().getTessellationIterations();
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v3_t c;
                c.x         = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y         = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                c.z         = 0;
                GLdouble *l = addTessVertex(c);
                gluTessVertex(_glu_tessellator, l, l);
            }
            coords.x = p3x;
            coords.y = p3y;

        } break;

        case (VG_QUAD_TO >> 1): {
            prev        = coords;
            VGfloat cpx = *coordsIter;
            coordsIter++;
            VGfloat cpy = *coordsIter;
            coordsIter++;
            VGfloat px = *coordsIter;
            coordsIter++;
            VGfloat py = *coordsIter;
            coordsIter++;

            if (isRelative) {
                cpx += prev.x;
                cpy += prev.y;
                px += prev.x;
                py += prev.y;
            }

            VGfloat increment = 1.0f / getContext().getTessellationIterations();
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v3_t c;
                c.x         = calcQuadBezier1d(coords.x, cpx, px, t);
                c.y         = calcQuadBezier1d(coords.y, cpy, py, t);
                c.z         = 0;
                GLdouble *l = addTessVertex(c);
                gluTessVertex(_glu_tessellator, l, l);
            }

            coords.x = px;
            coords.y = py;

        } break;

        case (VG_SCCWARC_TO >> 1):
        case (VG_SCWARC_TO >> 1):
        case (VG_LCCWARC_TO >> 1):
        case (VG_LCWARC_TO >> 1):

        {
            VGfloat rh = *coordsIter;
            coordsIter++;
            VGfloat rv = *coordsIter;
            coordsIter++;
            VGfloat rot = *coordsIter;
            coordsIter++;
            VGfloat cp1x = *coordsIter;
            coordsIter++;
            VGfloat cp1y = *coordsIter;
            coordsIter++;
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
                const int steps   = getContext().getTessellationIterations();
                VGfloat   beta    = 0; // angle. todo
                VGfloat   sinbeta = sinf(beta);
                VGfloat   cosbeta = cosf(beta);

                // calculate the start and end angles
                vertex_2d_t center;
                center.x = cx0[0];
                center.y = cx0[1];
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

                VGfloat cross = norm[0].x;

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
                for (VGfloat g = startAngle; g < endAngle; g += 360 / steps) {
                    v3_t c;

                    VGfloat alpha    = g * (M_PI / 180.0f);
                    VGfloat sinalpha = sinf(alpha);
                    VGfloat cosalpha = cosf(alpha);
                    c.x              = cx0[0] +
                          (rh * cosalpha * cosbeta - rv * sinalpha * sinbeta);
                    c.y = cx0[1] +
                          (rh * cosalpha * sinbeta + rv * sinalpha * cosbeta);
                    c.z         = 0;
                    GLdouble *l = addTessVertex(c);
                    gluTessVertex(_glu_tessellator, l, l);
                }
            }

            coords.x = cp1x;
            coords.y = cp1y;

        } break;

        default:
            throw std::runtime_error("Unsupported segment type");
            break;
        }
    } // foreach segment

    if (num_contours) {
        gluTessEndContour(_glu_tessellator);
        num_contours--;
    }

    if (num_contours != 0) {
        throw std::runtime_error("Error in tesselation");
    }

    gluTessEndPolygon(_glu_tessellator);

    // destroy the tesselator
    gluDeleteTess(_glu_tessellator);
    _glu_tessellator = nullptr;

    _out_vertices = nullptr;
    _out_bounding_box = nullptr;

}

void GLUTessellator::tessellate(IPath *path, std::vector<VGfloat> &vertices,
                                bounding_box_t &bounding_box) {
    throw std::runtime_error("Not implemented");
}

void GLUTessellator::tessBegin(GLenum type, GLvoid *user) {
    GLUTessellator *me = (GLUTessellator *)user;
    me->setPrimType(type);
    me->_vert_cnt = 0;
}

void GLUTessellator::tessEnd(GLvoid *user) {
    // DO NOTHING
}

void GLUTessellator::tessVertex(GLvoid *vertex, GLvoid *user) {
    GLUTessellator *me = (GLUTessellator *)user;
    GLdouble       *v  = (GLdouble *)vertex;

    if (me->primType() == GL_TRIANGLE_FAN) {
        // break up fans and strips into triangles
        switch (me->_vert_cnt) {
        case 0:
            me->_start_vert[0] = v[0];
            me->_start_vert[1] = v[1];
            break;
        case 1:
            me->_last_vert[0] = v[0];
            me->_last_vert[1] = v[1];
            break;

        default:
            me->addVertex(me->_start_vert);
            me->addVertex(me->_last_vert);
            me->addVertex(v);
            me->_last_vert[0] = v[0];
            me->_last_vert[1] = v[1];
            break;
        }
    } else if (me->primType() == GL_TRIANGLES) {
        me->addVertex(v);
    } else if (me->primType() == GL_TRIANGLE_STRIP) {
        switch (me->_vert_cnt) {
        case 0:
            me->addVertex(v);
            break;
        case 1:
            me->_start_vert[0] = v[0];
            me->_start_vert[1] = v[1];
            me->addVertex(v);
            break;
        case 2:
            me->_last_vert[0] = v[0];
            me->_last_vert[1] = v[1];
            me->addVertex(v);
            break;

        default:
            me->addVertex(me->_start_vert);
            me->addVertex(me->_last_vert);
            me->addVertex(v);
            me->_start_vert[0] = me->_last_vert[0];
            me->_start_vert[1] = me->_last_vert[1];
            me->_last_vert[0]  = v[0];
            me->_last_vert[1]  = v[1];
            break;
        }
    }
    me->_vert_cnt++;
}

void GLUTessellator::tessCombine(GLdouble coords[3], void *data[4],
                                 GLfloat weight[4], void **outData,
                                 void *polygonData) {

    GLUTessellator *me = (GLUTessellator *)polygonData;
    me->_tess_verts.push_back(v3_t(coords));
    *outData = &me->_tess_verts.back().x;
}

void GLUTessellator::tessError(GLenum errorCode) {
    std::cerr << "GLU tesselator error: " << errorCode << " "
              << gluErrorString(errorCode) << std::endl;
}

} // namespace MonkVG
