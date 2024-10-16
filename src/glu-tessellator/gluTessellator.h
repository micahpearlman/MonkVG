/**
 * @file gluTesselator.h
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __GLU_TESSELATOR_H__
#define __GLU_TESSELATOR_H__
#include "mkTessellator.h"
#include <GL/glu.h>
#include <vector>
#include <list>
#include <functional>
#include <array>
namespace MonkVG {
class GLUTessellator : public ITessellator {
  public:
    GLUTessellator(IContext &context);
    virtual ~GLUTessellator() = default;

    void tessellate(const std::vector<VGubyte> &segments,
                    const std::vector<VGfloat> &coords,
                    std::vector<VGfloat>       &vertices,
                    bounding_box_t             &bounding_box) override;

    void tessellate(IPath *path, std::vector<VGfloat> &vertices,
                    bounding_box_t &bounding_box) override;

  private:
    /**
     * @brief Used by the GLU tessellator to track vertices.
     *
     */
    struct v3_t {
        GLdouble x, y, z;
        v3_t() {}
        v3_t(GLdouble *v) : x(v[0]), y(v[1]), z(v[2]) {}
        v3_t(GLdouble ix, GLdouble iy, GLdouble iz) : x(ix), y(iy), z(iz) {}
    };

  private:
    // GLU Tessalator
    GLUtesselator *_glu_tessellator = nullptr;

    // storage for the tesselated vertices
    // NOTE: these verts are in 3D space
    // NOTE: the use of list is because we need pointers to items in the list
    // and vector will reallocate memory and invalidate the pointers
    std::list<v3_t> _tess_verts = {};
    GLdouble       *tessVerticesBackPtr() { return &(_tess_verts.back().x); }

    /// @brief Add a vertex to the tesselation vertex list
    /// @param v a 3D vertex
    /// @return pointer to the end of the vertex list
    GLdouble *addTessVertex(const v3_t &v) {
        // updateBounds(v.x, v.y);
        _tess_verts.push_back(v);
        return tessVerticesBackPtr();
    }

    // output vertices and bounding box
    std::vector<VGfloat> *_out_vertices     = nullptr;
    bounding_box_t       *_out_bounding_box = nullptr;

    void addVertex(const std::array<GLdouble, 2> &v) {
        const VGfloat x = static_cast<VGfloat>(v[0]);
        const VGfloat y = static_cast<VGfloat>(v[1]);
        _out_bounding_box->update(x, y);
        _out_vertices->push_back(x);
        _out_vertices->push_back(y);
    }

    // primitive type (GL_TRIANGLES, GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP,
    // GL_LINE_LOOP) NOTE: we convert everything to triangles.
    // See: tessVertex()
    GLenum       _prim_type = 0;
    const GLenum primType() const { return _prim_type; }
    void         setPrimType(GLenum t) { _prim_type = t; }

    // tesselation vertex tracking
    std::array<GLdouble, 2> _start_vert = {0.0, 0.0};
    std::array<GLdouble, 2> _last_vert  = {0.0, 0.0};
    int                     _vert_cnt   = 0;

    // GLU Tessalator callback functions
    static void tessBegin(GLenum type, GLvoid *user);
    static void tessEnd(GLvoid *user);
    static void tessVertex(GLvoid *vertex, GLvoid *user);
    static void tessCombine(GLdouble coords[3], void *data[4],
                            GLfloat weight[4], void **outData,
                            void *polygonData);
    static void tessError(GLenum errorCode);
    void        endOfTesselation(VGbitfield paintModes);

}; // GLUTessellator
} // namespace MonkVG
#endif // __GLU_TESSELATOR_H__
