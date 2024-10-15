/*
 *  glPath.cpp
 *  MonkVG-OpenGL
 *
 *  Created by Micah Pearlman on 8/6/10.
 *  Copyright 2010 MP Engineering. All rights reserved.
 *
 */

#include "glPath.h"
#include "glContext.h"
#include "glBatch.h"
#include <cassert>

namespace MonkVG {

OpenGLPath::OpenGLPath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale,
                       VGfloat bias, VGint segmentCapacityHint,
                       VGint coordCapacityHint, VGbitfield capabilities,
                       IContext &context)
    : IPath(pathFormat, datatype, scale, bias, segmentCapacityHint,
            coordCapacityHint, capabilities, context) {}

OpenGLPath::~OpenGLPath() {
    CHECK_GL_ERROR;

    // if (_fill_tess) {
    //     gluDeleteTess(_fill_tess);
    //     _fill_tess = nullptr;
    // }

    if (_fill_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_fill_vbo);
        _fill_vbo = GL_UNDEFINED;
    }

    if (_fill_vao != GL_UNDEFINED) {
        glDeleteVertexArrays(1, &_fill_vao);
        _fill_vao = GL_UNDEFINED;
    }

    if (_stroke_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_stroke_vbo);
        _stroke_vbo = GL_UNDEFINED;
    }

    if (_stroke_vao != GL_UNDEFINED) {
        glDeleteVertexArrays(1, &_stroke_vao);
        _stroke_vao = GL_UNDEFINED;
    }
    CHECK_GL_ERROR;
}

void OpenGLPath::clear(VGbitfield caps) {
    IPath::clear(caps);

    _fill_vertices.clear();

    // delete vbo buffers
    if (_stroke_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_stroke_vbo);
        _stroke_vbo = GL_UNDEFINED;
    }

    if (_fill_vbo != GL_UNDEFINED) {
        glDeleteBuffers(1, &_fill_vbo);
        _fill_vbo = GL_UNDEFINED;
    }
}

void OpenGLPath::buildFillIfDirty() {
    IPaint *current_fill_paint = IContext::instance().getFillPaint();
    if (current_fill_paint != _fill_paint) {
        _fill_paint    = (OpenGLPaint *)current_fill_paint;
        _is_fill_dirty = true;
    }
    // only build the fill if dirty or we are in batch build mode
    if (_is_fill_dirty || IContext::instance().currentBatch()) {
        buildFill();
    }
    _is_fill_dirty = false;
}

void OpenGLPath::buildStrokeIfDirty() {
    IPaint *current_stroke_paint = IContext::instance().getStrokePaint();
    if (current_stroke_paint != _stroke_paint) {
        _stroke_paint    = (OpenGLPaint *)current_stroke_paint;
        _is_stroke_dirty = true;
    }
    // only build the fill if dirty or we are in batch build mode
    if (_is_stroke_dirty || IContext::instance().currentBatch()) {
        buildStroke();
    }
    _is_stroke_dirty = false;
}

void printMat44(float m[4][4]) {
    printf("--\n");
    for (int x = 0; x < 4; x++) {
        printf("%f\t%f\t%f\t%f\n", m[x][0], m[x][1], m[x][2], m[x][3]);
    }
}

bool OpenGLPath::draw(VGbitfield paint_modes) {

    if (paint_modes == 0)
        return false;

    CHECK_GL_ERROR;

    // get the native OpenGL context
    OpenGLContext &gl_ctx = (MonkVG::OpenGLContext &)IContext::instance();

    if (paint_modes & VG_FILL_PATH) { // build the fill polygons
        buildFillIfDirty();
    }

    if (paint_modes & VG_STROKE_PATH) {
        buildStrokeIfDirty();
    }

    endOfTesselation(paint_modes);

    if (gl_ctx.currentBatch()) {
        return true; // creating a batch so bail from here
    }

    // restore image mode later
    VGImageMode old_image_mode = gl_ctx.getImageMode();

    // configure based on paint type
    if (_fill_paint && _fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
        // set the shader to a color shader
        gl_ctx.bindShader(OpenGLContext::ShaderType::ColorShader);

        // context will setup any uniforms
        IContext::instance().fill();

        // bind the vao & vbo and draw
        glBindVertexArray(_fill_vao);
        glDrawArrays(GL_TRIANGLES, 0, _num_fill_verts);
        glBindVertexArray(0);

    } else if (_fill_paint &&
               (_fill_paint->getPaintType() == VG_PAINT_TYPE_LINEAR_GRADIENT ||
                _fill_paint->getPaintType() == VG_PAINT_TYPE_RADIAL_GRADIENT ||
                _fill_paint->getPaintType() ==
                    VG_PAINT_TYPE_RADIAL_2x3_GRADIENT ||
                _fill_paint->getPaintType() ==
                    VG_PAINT_TYPE_LINEAR_2x3_GRADIENT)) {
        throw std::runtime_error(
            "not implemented"); // TODO: do gradients in shader, implement
                                // texture shaders

        gl_ctx.setImageMode(VG_DRAW_IMAGE_NORMAL);
    }

    // this is important to unbind the vbo when done
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw the stroke last so it renders on top of fill
    if ((paint_modes & VG_STROKE_PATH) && _stroke_vao != GL_UNDEFINED) {
        if (_stroke_paint &&
            _stroke_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
            // set the shader to a color shader
            gl_ctx.bindShader(OpenGLContext::ShaderType::ColorShader);

        } else {
            throw std::runtime_error("Non color stroke paint not implemented");
        }
        // draw
        IContext::instance().stroke();
        glBindVertexArray(_stroke_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _num_stroke_verts);
        glBindVertexArray(0);
    }

    CHECK_GL_ERROR;

    return true;
}

void OpenGLPath::buildFill() {
    getContext().getTessellator().tessellate(_segments, _fcoords, _fill_vertices, _bounds);
#if 0
    _fill_vertices.clear();

    // reset the bounds
    _min_x  = VG_MAX_FLOAT;
    _min_y  = VG_MAX_FLOAT;
    _width  = -VG_MAX_FLOAT;
    _height = -VG_MAX_FLOAT;

    CHECK_GL_ERROR;

    _fill_tess = gluNewTess();
    gluTessCallback(_fill_tess, GLU_TESS_BEGIN_DATA,
                    (GLvoid(APIENTRY *)()) & OpenGLPath::tessBegin);
    gluTessCallback(_fill_tess, GLU_TESS_END_DATA,
                    (GLvoid(APIENTRY *)()) & OpenGLPath::tessEnd);
    gluTessCallback(_fill_tess, GLU_TESS_VERTEX_DATA,
                    (GLvoid(APIENTRY *)()) & OpenGLPath::tessVertex);
    gluTessCallback(_fill_tess, GLU_TESS_COMBINE_DATA,
                    (GLvoid(APIENTRY *)()) & OpenGLPath::tessCombine);
    gluTessCallback(_fill_tess, GLU_TESS_ERROR,
                    (GLvoid(APIENTRY *)()) & OpenGLPath::tessError);
    if (IContext::instance().getFillRule() == VG_EVEN_ODD) {
        gluTessProperty(_fill_tess, GLU_TESS_WINDING_RULE,
                        GLU_TESS_WINDING_ODD);
    } else if (IContext::instance().getFillRule() == VG_NON_ZERO) {
        gluTessProperty(_fill_tess, GLU_TESS_WINDING_RULE,
                        GLU_TESS_WINDING_NONZERO);
    }
    gluTessProperty(_fill_tess, GLU_TESS_TOLERANCE, 0.5f);

    gluTessBeginPolygon(_fill_tess, this);

    std::vector<VGfloat>::iterator coordsIter = _fcoords.begin();
    VGbyte                         segment    = VG_CLOSE_PATH;
    v3_t                           coords(0, 0, 0);
    v3_t                           prev(0, 0, 0);
    int                            num_contours = 0;

    for (auto segment : _segments) {

        //			VG_CLOSE_PATH                               = (
        // 0 << 1), 			VG_MOVE_TO = ( 1 << 1),
        // VG_LINE_TO = ( 2 << 1), 			VG_HLINE_TO = ( 3 << 1),
        // VG_VLINE_TO = ( 4 << 1), 			VG_QUAD_TO = ( 5 << 1),
        // VG_CUBIC_TO = ( 6 << 1), 			VG_SQUAD_TO = ( 7 << 1),
        // VG_SCUBIC_TO =
        //( 8 << 1), 			VG_SCCWARC_TO = ( 9 << 1),
        // VG_SCWARC_TO = (10 << 1), 			VG_LCCWARC_TO = (11 <<
        // 1), 			VG_LCWARC_TO = (12 << 1),

        // todo: deal with relative move
        bool isRelative = segment & VG_RELATIVE;
        switch (segment >> 1) {
        case (VG_CLOSE_PATH >> 1): {
            if (num_contours) {
                gluTessEndContour(_fill_tess);
                num_contours--;
            }

        } break;
        case (VG_MOVE_TO >> 1): {
            if (num_contours) {
                gluTessEndContour(_fill_tess);
                num_contours--;
            }

            gluTessBeginContour(_fill_tess);
            num_contours++;
            coords.x = *coordsIter;
            coordsIter++;
            coords.y = *coordsIter;
            coordsIter++;

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_fill_tess, l, l);

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
            gluTessVertex(_fill_tess, l, l);
        } break;
        case (VG_HLINE_TO >> 1): {
            prev     = coords;
            coords.x = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.x += prev.x;
            }

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_fill_tess, l, l);
        } break;
        case (VG_VLINE_TO >> 1): {
            prev     = coords;
            coords.y = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.y += prev.y;
            }

            GLdouble *l = addTessVertex(coords);
            gluTessVertex(_fill_tess, l, l);
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

            VGfloat increment =
                1.0f / IContext::instance().getTessellationIterations();
            // printf("\tcubic: ");
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v3_t c;
                c.x         = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y         = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                c.z         = 0;
                GLdouble *l = addTessVertex(c);
                gluTessVertex(_fill_tess, l, l);
                //	c.print();
            }
            // printf("\n");
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

            VGfloat increment =
                1.0f / IContext::instance().getTessellationIterations();
            // printf("\tcubic: ");
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v3_t c;
                c.x         = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y         = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                c.z         = 0;
                GLdouble *l = addTessVertex(c);
                gluTessVertex(_fill_tess, l, l);
                //	c.print();
            }
            // printf("\n");
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

            VGfloat increment =
                1.0f / IContext::instance().getTessellationIterations();
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v3_t c;
                c.x         = calcQuadBezier1d(coords.x, cpx, px, t);
                c.y         = calcQuadBezier1d(coords.y, cpy, py, t);
                c.z         = 0;
                GLdouble *l = addTessVertex(c);
                gluTessVertex(_fill_tess, l, l);
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
                const int steps =
                    IContext::instance().getTessellationIterations();
                VGfloat beta    = 0; // angle. todo
                VGfloat sinbeta = sinf(beta);
                VGfloat cosbeta = cosf(beta);

                // calculate the start and end angles
                v2_t center;
                center.x = cx0[0];
                center.y = cx0[1];
                v2_t norm[2];
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
                    gluTessVertex(_fill_tess, l, l);
                }
            }

            coords.x = cp1x;
            coords.y = cp1y;

        } break;

        default:
            printf("unkwown command\n");
            break;
        }
    } // foreach segment

    if (num_contours) {
        gluTessEndContour(_fill_tess);
        num_contours--;
    }

    assert(num_contours == 0);

    gluTessEndPolygon(_fill_tess);

    gluDeleteTess(_fill_tess);
    _fill_tess = 0;

    // final calculation of the width and height
    _width  = fabsf(_width - _min_x);
    _height = fabsf(_height - _min_y);
#endif // 0
    CHECK_GL_ERROR;
}

void OpenGLPath::buildFatLineSegment(std::vector<v2_t> &vertices,
                                     const v2_t &p0, const v2_t &p1,
                                     const float stroke_width) {

    if ((p0.x == p1.x) && (p0.y == p1.y)) {
        return;
    }

    float       dx      = p1.y - p0.y;
    float       dy      = p0.x - p1.x;
    const float inv_mag = 1.0f / sqrtf(dx * dx + dy * dy);
    dx                  = dx * inv_mag;
    dy                  = dy * inv_mag;

    v2_t        v0, v1, v2, v3;
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

void OpenGLPath::buildStroke() {
    _stroke_verts.clear();

    // get the native OpenGL context
    OpenGLContext &gl_ctx = (MonkVG::OpenGLContext &)IContext::instance();

    const VGfloat stroke_width = gl_ctx.getStrokeLineWidth();

    std::vector<VGfloat>::iterator coordsIter = _fcoords.begin();
    VGbyte                         segment    = VG_CLOSE_PATH;
    v2_t                           coords     = {0, 0};
    v2_t                           prev       = {0, 0};
    v2_t                           closeTo    = {0, 0};
    for (const auto &segment : _segments) {

        // todo: deal with relative move
        bool isRelative = segment & VG_RELATIVE;
        switch (segment >> 1) {
        case (VG_CLOSE_PATH >> 1): {
            buildFatLineSegment(_stroke_verts, coords, closeTo, stroke_width);
        } break;
        case (VG_MOVE_TO >> 1): {
            prev.x = closeTo.x = coords.x = *coordsIter;
            coordsIter++;
            prev.y = closeTo.y = coords.y = *coordsIter;
            coordsIter++;

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

            buildFatLineSegment(_stroke_verts, prev, coords, stroke_width);

        } break;
        case (VG_HLINE_TO >> 1): {
            prev     = coords;
            coords.x = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.x += prev.x;
            }

            buildFatLineSegment(_stroke_verts, prev, coords, stroke_width);
        } break;
        case (VG_VLINE_TO >> 1): {
            prev     = coords;
            coords.y = *coordsIter;
            coordsIter++;
            if (isRelative) {
                coords.y += prev.y;
            }

            buildFatLineSegment(_stroke_verts, prev, coords, stroke_width);

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

            VGfloat increment =
                1.0f / IContext::instance().getTessellationIterations();
            // printf("\tcubic: ");
            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v2_t c;
                c.x = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                buildFatLineSegment(_stroke_verts, prev, c, stroke_width);
                prev = c;
            }
            coords.x = p3x;
            coords.y = p3y;

        } break;

        case (VG_QUAD_TO >> 1): // added by rhcad
        {
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

            VGfloat increment =
                1.0f / IContext::instance().getTessellationIterations();

            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v2_t c;
                c.x = calcQuadBezier1d(coords.x, cpx, px, t);
                c.y = calcQuadBezier1d(coords.y, cpy, py, t);
                buildFatLineSegment(_stroke_verts, prev, c, stroke_width);
                prev = c;
            }
            coords.x = px;
            coords.y = py;

        } break;

        case (VG_CUBIC_TO >> 1): // todo
        {
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

            VGfloat increment =
                1.0f / IContext::instance().getTessellationIterations();

            for (VGfloat t = increment; t < 1.0f + increment; t += increment) {
                v2_t c;
                c.x = calcCubicBezier1d(coords.x, cp1x, cp2x, p3x, t);
                c.y = calcCubicBezier1d(coords.y, cp1y, cp2y, p3y, t);
                buildFatLineSegment(_stroke_verts, prev, c, stroke_width);
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
                const int steps =
                    IContext::instance().getTessellationIterations();
                VGfloat beta    = 0; // angle. todo
                VGfloat sinbeta = sinf(beta);
                VGfloat cosbeta = cosf(beta);

                // calculate the start and end angles
                v2_t center;
                center.x = cx0[0]; //(cx0[0] + cx1[0])*0.5f;
                center.y = cx0[1]; //(cx0[1] + cx1[1])*0.5f;
                v2_t norm[2];
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
                    v2_t c;

                    VGfloat alpha    = g * (M_PI / 180.0f);
                    VGfloat sinalpha = sinf(alpha);
                    VGfloat cosalpha = cosf(alpha);
                    c.x              = cx0[0] +
                          (rh * cosalpha * cosbeta - rv * sinalpha * sinbeta);
                    c.y = cx0[1] +
                          (rh * cosalpha * sinbeta + rv * sinalpha * cosbeta);
                    // printf( "(%f, %f)\n", c[0], c[1] );
                    buildFatLineSegment(_stroke_verts, prev, c, stroke_width);
                    prev = c;
                }
            }

            coords.x = cp1x;
            coords.y = cp1y;

        } break;

        default:
            printf("unkwown command: %d\n", segment >> 1);
            break;
        }
    } // foreach segment
}

void OpenGLPath::endOfTesselation(VGbitfield paint_modes) {
    /// build fill vao & vbo
    if (_fill_vertices.size() > 0) {

        // delete the old vbo
        if (_fill_vbo != GL_UNDEFINED) {
            glDeleteBuffers(1, &_fill_vbo);
            _fill_vbo = GL_UNDEFINED;
        }

        // delete the old vao
        if (_fill_vao != GL_UNDEFINED) {
            glDeleteVertexArrays(1, &_fill_vao);
            _fill_vao = GL_UNDEFINED;
        }

        glGenVertexArrays(1, &_fill_vao);
        glGenBuffers(1, &_fill_vbo);
        glBindVertexArray(_fill_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _fill_vbo);
        if (_fill_paint && _fill_paint->getPaintType() == VG_PAINT_TYPE_COLOR) {
            glBufferData(GL_ARRAY_BUFFER,
                         _fill_vertices.size() * sizeof(VGfloat),
                         _fill_vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
            glEnableVertexAttribArray(0);
        } else if (_fill_paint && (_fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_LINEAR_GRADIENT ||
                                   _fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_RADIAL_GRADIENT ||
                                   _fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_RADIAL_2x3_GRADIENT ||
                                   _fill_paint->getPaintType() ==
                                       VG_PAINT_TYPE_LINEAR_2x3_GRADIENT)) {
            throw std::runtime_error("Not implemented");
            std::vector<textured_vertex_t> texturedVertices;
            for (std::vector<float>::const_iterator it = _fill_vertices.begin();
                 it != _fill_vertices.end(); it++) {
                // build up the textured vertex
                textured_vertex_t v;
                v.v[0] = *it;
                it++;
                v.v[1]  = *it;
                v.uv[0] = fabsf(v.v[0] - getMinX()) / getWidth();
                v.uv[1] = fabsf(v.v[1] - getMinY()) / getHeight();
                texturedVertices.push_back(v);
            }

            glBufferData(GL_ARRAY_BUFFER,
                         texturedVertices.size() * sizeof(textured_vertex_t),
                         &texturedVertices[0], GL_STATIC_DRAW);

            texturedVertices.clear();

            // setup the paints linear gradient
            _fill_paint->buildGradientImage(getWidth(), getHeight());
        }

        _num_fill_verts = (int)_fill_vertices.size() / 2;
        // _tess_verts.clear();
    }

    /// build stroke vbo
    if (_stroke_verts.size() > 0) {
        // build the vertex buffer object VBO
        if (_stroke_vbo != GL_UNDEFINED) {
            glDeleteBuffers(1, &_stroke_vbo);
            _stroke_vbo = GL_UNDEFINED;
        }

        if (_stroke_vao != GL_UNDEFINED) {
            glDeleteVertexArrays(1, &_stroke_vao);
            _stroke_vao = GL_UNDEFINED;
        }
        glGenVertexArrays(1, &_stroke_vao);
        glGenBuffers(1, &_stroke_vbo);
        glBindVertexArray(_stroke_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _stroke_vbo);
        glBufferData(GL_ARRAY_BUFFER, _stroke_verts.size() * sizeof(v2_t),
                     _stroke_verts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glEnableVertexAttribArray(0);

        _num_stroke_verts = (int)_stroke_verts.size();
    }

    OpenGLBatch *glBatch = (OpenGLBatch *)IContext::instance().currentBatch();
    if (glBatch) { // if in batch mode update the current batch
        glBatch->addPathVertexData(
            &_fill_vertices[0], _fill_vertices.size() / 2,
            (float *)&_stroke_verts[0], _stroke_verts.size(), paint_modes);
    }

    // clear out vertex buffer
    _fill_vertices.clear();
    _stroke_verts.clear();
}

// static GLdouble startVertex_[2];
// static GLdouble lastVertex_[2];
// static int      vertexCount_ = 0;

// void OpenGLPath::tessBegin(GLenum type, GLvoid *user) {
//     OpenGLPath *me = (OpenGLPath *)user;
//     me->setPrimType(type);
//     vertexCount_ = 0;

//     switch (type) {
//     case GL_TRIANGLES:
//         // printf( "begin(GL_TRIANGLES)\n" );
//         break;
//     case GL_TRIANGLE_FAN:
//         // printf( "begin(GL_TRIANGLE_FAN)\n" );
//         break;
//     case GL_TRIANGLE_STRIP:
//         // printf( "begin(GL_TRIANGLE_STRIP)\n" );
//         break;
//     case GL_LINE_LOOP:
//         // printf( "begin(GL_LINE_LOOP)\n" );
//         break;
//     default:
//         break;
//     }
// }

// void OpenGLPath::tessEnd(GLvoid *user) {
//     //		OpenGLPath* me = (OpenGLPath*)user;
//     //		me->endOfTesselation();

//     // printf("end\n");
// }

// void OpenGLPath::tessVertex(GLvoid *vertex, GLvoid *user) {
//     OpenGLPath *me = (OpenGLPath *)user;
//     GLdouble   *v  = (GLdouble *)vertex;

//     if (me->primType() == GL_TRIANGLE_FAN) {
//         // break up fans and strips into triangles
//         switch (vertexCount_) {
//         case 0:
//             startVertex_[0] = v[0];
//             startVertex_[1] = v[1];
//             break;
//         case 1:
//             lastVertex_[0] = v[0];
//             lastVertex_[1] = v[1];
//             break;

//         default:
//             me->addVertex(startVertex_);
//             me->addVertex(lastVertex_);
//             me->addVertex(v);
//             lastVertex_[0] = v[0];
//             lastVertex_[1] = v[1];
//             break;
//         }
//     } else if (me->primType() == GL_TRIANGLES) {
//         me->addVertex(v);
//     } else if (me->primType() == GL_TRIANGLE_STRIP) {
//         switch (vertexCount_) {
//         case 0:
//             me->addVertex(v);
//             break;
//         case 1:
//             startVertex_[0] = v[0];
//             startVertex_[1] = v[1];
//             me->addVertex(v);
//             break;
//         case 2:
//             lastVertex_[0] = v[0];
//             lastVertex_[1] = v[1];
//             me->addVertex(v);
//             break;

//         default:
//             me->addVertex(startVertex_);
//             me->addVertex(lastVertex_);
//             me->addVertex(v);
//             startVertex_[0] = lastVertex_[0];
//             startVertex_[1] = lastVertex_[1];
//             lastVertex_[0]  = v[0];
//             lastVertex_[1]  = v[1];
//             break;
//         }
//     }
//     vertexCount_++;

//     // printf("\tvert[%d]: %f, %f, %f\n", vertexCount_, v[0], v[1], v[2] );
// }
// void OpenGLPath::tessCombine(GLdouble coords[3], void *data[4],
//                              GLfloat weight[4], void **outData,
//                              void *polygonData) {

//     OpenGLPath *me = (OpenGLPath *)polygonData;
//     *outData       = me->addTessVertex(coords);
// }

// void OpenGLPath::tessError(GLenum errorCode) {
//     printf("tesselator error: [%d] %s\n", errorCode,
//     gluErrorString(errorCode));
// }

} // namespace MonkVG
