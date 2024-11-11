/*
 *  mkMath.h
 *  MonkVG-Quartz
 *
 *  Created by Micah Pearlman on 3/11/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkMath_h__
#define __mkMath_h__

#include <MonkVG/openvg.h>
#include <cmath>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "mkCommon.h"
#include "mkTypes.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace MonkVG {

static inline VGfloat radians(VGfloat degrees) {
    return (VGfloat)(degrees * (M_PI / 180.0f));
}
static inline VGfloat degrees(VGfloat radians) {
    return (VGfloat)(radians * (180.0f / M_PI));
}

static inline VGfloat calcCubicBezier1d(VGfloat x0, VGfloat x1, VGfloat x2,
                                        VGfloat x3, VGfloat t) {
    // see openvg 1.0 spec section 8.3.2 Cubic Bezier Curves
    VGfloat one_t = 1.0f - t;
    VGfloat x = x0 * (one_t * one_t * one_t) + 3.0f * x1 * (one_t * one_t) * t +
                3.0f * x2 * one_t * (t * t) + x3 * (t * t * t);
    return x;
}

static inline VGfloat calcQuadBezier1d(VGfloat start, VGfloat control,
                                       VGfloat end, VGfloat time) {
    float inverse_time = 1.0f - time;
    return (powf(inverse_time, 2.0f) * start) +
           (2.0f * inverse_time * time * control) + (powf(time, 2.0f) * end);
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Given: Points (x0, y0) and (x1, y1)
// Return: TRUE if a solution exists, FALSE otherwise
//	Circle centers are written to (cx0, cy0) and (cx1, cy1)
static VGboolean findUnitCircles(VGfloat x0, VGfloat y0, VGfloat x1, VGfloat y1,
                                 VGfloat *cx0, VGfloat *cy0, VGfloat *cx1,
                                 VGfloat *cy1) {

    // Compute differences and averages
    VGfloat dx = x0 - x1;
    VGfloat dy = y0 - y1;
    VGfloat xm = (x0 + x1) / 2;
    VGfloat ym = (y0 + y1) / 2;
    VGfloat dsq, disc, s, sdx, sdy;
    // Solve for intersecting unit circles
    dsq = dx * dx + dy * dy;
    if (dsq == 0.0)
        return VG_FALSE; // Points are coincident
    disc = 1.0f / dsq - 1.0f / 4.0f;
    if (disc < 0.0)
        return VG_FALSE; // Points are too far apart

    s    = sqrt(disc);
    sdx  = s * dx;
    sdy  = s * dy;
    *cx0 = xm + sdy;
    *cy0 = ym - sdx;
    *cx1 = xm - sdy;
    *cy1 = ym + sdx;

    return VG_TRUE;
}

// Given:
// Return:
// Ellipse parameters rh, rv, rot (in degrees), endpoints (x0, y0) and (x1, y1)
// TRUE if a solution exists, FALSE otherwise. Ellipse centers are written to
// (cx0, cy0) and (cx1, cy1)

static VGboolean findEllipses(VGfloat rh, VGfloat rv, VGfloat rot, VGfloat x0,
                              VGfloat y0, VGfloat x1, VGfloat y1, VGfloat *cx0,
                              VGfloat *cy0, VGfloat *cx1, VGfloat *cy1) {
    VGfloat COS, SIN, x0p, y0p, x1p, y1p, pcx0, pcy0, pcx1, pcy1;
    // Convert rotation angle from degrees to radians
    rot *= M_PI / 180.0;
    // Pre-compute rotation matrix entries
    COS = cos(rot);
    SIN = sin(rot);
    // Transform (x0, y0) and (x1, y1) into unit space
    // using (inverse) rotate, followed by (inverse) scale
    x0p = (x0 * COS + y0 * SIN) / rh;
    y0p = (-x0 * SIN + y0 * COS) / rv;
    x1p = (x1 * COS + y1 * SIN) / rh;
    y1p = (-x1 * SIN + y1 * COS) / rv;
    if (!findUnitCircles(x0p, y0p, x1p, y1p, &pcx0, &pcy0, &pcx1, &pcy1)) {
        return VG_FALSE;
    }
    // Transform back to original coordinate space
    // using (forward) scale followed by (forward) rotate
    pcx0 *= rh;
    pcy0 *= rv;
    pcx1 *= rh;
    pcy1 *= rv;
    *cx0 = pcx0 * COS - pcy0 * SIN;
    *cy0 = pcx0 * SIN + pcy0 * COS;
    *cx1 = pcx1 * COS - pcy1 * SIN;
    *cy1 = pcx1 * SIN + pcy1 * COS;

    return VG_TRUE;
}

//	[ sx	shx	tx
//	 shy	sy	ty
//	 0		0	1 ]
//	sx and sy define scaling in the x and y directions, respectively;
//	shx and shy define shearing in the x and y directions, respectively;
//	tx and ty define translation in the x and y directions, respectively.

class Matrix33 {
  public:
    union {
        struct {
            float a, c, e,     // cos(a) -sin(a) tx
                b, d, f,       // sin(a) cos(a)  ty
                ff0, ff1, ff2; // 0      0       1
        };
        float m[9];
        float mm[3][3];
    };

    Matrix33(float *t) {
        a   = t[0];
        c   = t[1];
        e   = t[2];
        b   = t[3];
        d   = t[4];
        f   = t[5];
        ff0 = t[6];
        ff1 = t[7];
        ff2 = t[8];
    }

    void setIdentity() {
        // set to identity
        a = d = ff2 = 1.0f;
        c = e = b = f = ff0 = ff1 = 0;
    }

    void setTranslate(float x, float y) {
        e = x;
        f = y;
    }

    void setScale(float sx, float sy) {
        a = sx;
        d = sy;
    }

    void setRotation(float ang) { // assume radians
        float cs = cosf(ang);
        float ss = sinf(ang);
        a        = cs;
        c        = -ss;
        b        = ss;
        d        = cs;
    }

    void translation(float t[2]) const {
        t[0] = e;
        t[1] = f;
    }
    void setTranslation(float t[2]) {
        e = t[0];
        f = t[1];
    }
    float angle() const { return acosf(a); }
    void  setAngle(float ang) { setRotation(ang); }

    void setShear(float shx, float shy) {
        a = d = 1.0f;
        b = shx;
        c = shy;
    }

    void lookAt(float la[2]) const {
        la[0] = a;
        la[1] = b;
    }

    float *ptr() { return &a; }

    static void multiply(Matrix33 &r, const Matrix33 &a, const Matrix33 &b) {
        for (int z = 0; z < 9; z++)
            r.m[z] = 0;

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                for (int k = 0; k < 3; k++) {
                    r.mm[i][j] += a.mm[i][k] * b.mm[k][j];
                }
    }

    void print() {
        std::cout << ":: Matrix33 ::" << std::endl;
        for (int i = 0; i < 3; i++) {
            for (int p = 0; p < 3; p++) {
                std::cout << std::setw(6) << std::setiosflags(std::ios::fixed)
                          << std::setprecision(3) << mm[i][p];
            }
            std::cout << std::endl;
        }
    }

    Matrix33() { setIdentity(); }

    Matrix33(const Matrix33 &m) { this->copy(m); }

    inline void set(int row, int col, VGfloat v) { mm[row][col] = v; }

    inline VGfloat get(int row, int col) const { return mm[row][col]; }

    inline void copy(const Matrix33 &m_) {
        for (int i = 0; i < 9; i++)
            m[i] = m_.m[i];
    }
    inline void copy(float *o) {
        for (int i = 0; i < 9; i++)
            m[i] = o[i];
    }
    inline void transpose() {
        Matrix33 tmp;
        for (int i = 0; i < 3; i++)
            for (int k = 0; k < 3; k++)
                tmp.set(i, k, get(k, i));
        copy(tmp);
    }
    inline void postMultiply(const Matrix33 &m) {
        Matrix33 tmp;
        Matrix33::multiply(tmp, *this, m);
        copy(tmp);
    }

    inline void preMultiply(const Matrix33 &m) {
        Matrix33 tmp;
        Matrix33::multiply(tmp, m, *this);
        copy(tmp);
    }
};

inline void affineTransform(float result[2], const Matrix33 &m,
                            const float v[2]) {
    result[0] = v[0] * m.get(0, 0) + v[1] * m.get(0, 1) + m.get(0, 2);
    result[1] = v[0] * m.get(1, 0) + v[1] * m.get(1, 1) + m.get(1, 2);
}

/// Gradient helper functions
/**
 * @brief Calculate the stops surrounding the given gradient position.
 * @param stop0 stop before the position
 * @param stop1 stop after the position
 * @param g gradient position (between 0 and 1 inclusive)
 */
void calcStops(const std::vector<gradient_stop_t> &stops,
               gradient_stop_t &stop0, gradient_stop_t &stop1, float g);

/**
 * @brief linearly interpolate between two stops
 *
 * @param dst the final color
 * @param stop0 the first stop
 * @param stop1 the second stop
 * @param g the gradient position
 */
void lerpStops(color_t &dst, const gradient_stop_t &stop0,
               const gradient_stop_t &stop1, const float g);

} // namespace MonkVG

#endif // __mkMath_h__