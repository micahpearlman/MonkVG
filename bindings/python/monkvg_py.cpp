/**
 * @file monkvg_py.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Python bindings for MonkVG
 * @version 0.1
 * @date 2024-11-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <pybind11/pybind11.h>
#include <MonkVG/openvg.h>
#include <MonkVG/vgext.h>
#include <MonkVG/vgu.h>

#include <vector>

namespace py = pybind11;

int add(int i, int j) { return i + j; }

PYBIND11_MODULE(monkvg_py, m) {
    m.def("add", &add, "A function that adds two numbers");

    /// Enumerations
    py::enum_<VGboolean>(m, "VGboolean")
        .value("VG_FALSE", VG_FALSE)
        .value("VG_TRUE", VG_TRUE)
        .export_values();

    py::enum_<VGRenderingBackendTypeMNK>(m, "VGRenderingBackendTypeMNK")
        .value("VG_RENDERING_BACKEND_TYPE_OPENGLES11",
               VG_RENDERING_BACKEND_TYPE_OPENGLES11)
        .value("VG_RENDERING_BACKEND_TYPE_OPENGLES20",
               VG_RENDERING_BACKEND_TYPE_OPENGLES20)
        .value("VG_RENDERING_BACKEND_TYPE_OPENGLES32",
               VG_RENDERING_BACKEND_TYPE_OPENGLES32)
        .value("VG_RENDERING_BACKEND_TYPE_OPENGL33",
               VG_RENDERING_BACKEND_TYPE_OPENGL33)
        .value("VG_RENDERING_BACKEND_TYPE_VULKAN",
               VG_RENDERING_BACKEND_TYPE_VULKAN)
        .value("VG_RENDERING_BACKEND_TYPE_FORCE_SIZE",
               VG_RENDERING_BACKEND_TYPE_FORCE_SIZE)
        .export_values();

    py::enum_<VGErrorCode>(m, "VGErrorCode")
        .value("VG_NO_ERROR", VG_NO_ERROR)
        .value("VG_BAD_HANDLE_ERROR", VG_BAD_HANDLE_ERROR)
        .value("VG_ILLEGAL_ARGUMENT_ERROR", VG_ILLEGAL_ARGUMENT_ERROR)
        .value("VG_OUT_OF_MEMORY_ERROR", VG_OUT_OF_MEMORY_ERROR)
        .value("VG_PATH_CAPABILITY_ERROR", VG_PATH_CAPABILITY_ERROR)
        .value("VG_UNSUPPORTED_IMAGE_FORMAT_ERROR",
               VG_UNSUPPORTED_IMAGE_FORMAT_ERROR)
        .value("VG_UNSUPPORTED_PATH_FORMAT_ERROR",
               VG_UNSUPPORTED_PATH_FORMAT_ERROR)
        .value("VG_IMAGE_IN_USE_ERROR", VG_IMAGE_IN_USE_ERROR)
        .value("VG_NO_CONTEXT_ERROR", VG_NO_CONTEXT_ERROR)
        .value("VG_ERROR_CODE_FORCE_SIZE", VG_ERROR_CODE_FORCE_SIZE)
        .export_values();

    py::enum_<VGParamType>(m, "VGParamType")
        .value("VG_MATRIX_MODE", VG_MATRIX_MODE)
        .value("VG_FILL_RULE", VG_FILL_RULE)
        .value("VG_IMAGE_QUALITY", VG_IMAGE_QUALITY)
        .value("VG_RENDERING_QUALITY", VG_RENDERING_QUALITY)
        .value("VG_BLEND_MODE", VG_BLEND_MODE)
        .value("VG_IMAGE_MODE", VG_IMAGE_MODE)
        .value("VG_SCISSOR_RECTS", VG_SCISSOR_RECTS)
        .value("VG_COLOR_TRANSFORM", VG_COLOR_TRANSFORM)
        .value("VG_COLOR_TRANSFORM_VALUES", VG_COLOR_TRANSFORM_VALUES)
        .value("VG_STROKE_LINE_WIDTH", VG_STROKE_LINE_WIDTH)
        .value("VG_STROKE_CAP_STYLE", VG_STROKE_CAP_STYLE)
        .value("VG_STROKE_JOIN_STYLE", VG_STROKE_JOIN_STYLE)
        .value("VG_STROKE_MITER_LIMIT", VG_STROKE_MITER_LIMIT)
        .value("VG_STROKE_DASH_PATTERN", VG_STROKE_DASH_PATTERN)
        .value("VG_STROKE_DASH_PHASE", VG_STROKE_DASH_PHASE)
        .value("VG_STROKE_DASH_PHASE_RESET", VG_STROKE_DASH_PHASE_RESET)
        .value("VG_TILE_FILL_COLOR", VG_TILE_FILL_COLOR)
        .value("VG_CLEAR_COLOR", VG_CLEAR_COLOR)
        .value("VG_GLYPH_ORIGIN", VG_GLYPH_ORIGIN)
        .value("VG_MASKING", VG_MASKING)
        .value("VG_SCISSORING", VG_SCISSORING)
        .value("VG_PIXEL_LAYOUT", VG_PIXEL_LAYOUT)
        .value("VG_SCREEN_LAYOUT", VG_SCREEN_LAYOUT)
        .value("VG_FILTER_FORMAT_LINEAR", VG_FILTER_FORMAT_LINEAR)
        .value("VG_FILTER_FORMAT_PREMULTIPLIED", VG_FILTER_FORMAT_PREMULTIPLIED)
        .value("VG_FILTER_CHANNEL_MASK", VG_FILTER_CHANNEL_MASK)
        .value("VG_MAX_SCISSOR_RECTS", VG_MAX_SCISSOR_RECTS)
        .value("VG_MAX_DASH_COUNT", VG_MAX_DASH_COUNT)
        .value("VG_MAX_KERNEL_SIZE", VG_MAX_KERNEL_SIZE)
        .value("VG_MAX_SEPARABLE_KERNEL_SIZE", VG_MAX_SEPARABLE_KERNEL_SIZE)
        .value("VG_MAX_COLOR_RAMP_STOPS", VG_MAX_COLOR_RAMP_STOPS)
        .value("VG_MAX_IMAGE_WIDTH", VG_MAX_IMAGE_WIDTH)
        .value("VG_MAX_IMAGE_HEIGHT", VG_MAX_IMAGE_HEIGHT)
        .value("VG_MAX_IMAGE_PIXELS", VG_MAX_IMAGE_PIXELS)
        .value("VG_MAX_IMAGE_BYTES", VG_MAX_IMAGE_BYTES)
        .value("VG_MAX_FLOAT", VG_MAX_FLOAT)
        .value("VG_MAX_GAUSSIAN_STD_DEVIATION", VG_MAX_GAUSSIAN_STD_DEVIATION)
        .export_values();

    py::enum_<VGRenderingQuality>(m, "VGRenderingQuality")
        .value("VG_RENDERING_QUALITY_NONANTIALIASED",
               VG_RENDERING_QUALITY_NONANTIALIASED)
        .value("VG_RENDERING_QUALITY_FASTER", VG_RENDERING_QUALITY_FASTER)
        .value("VG_RENDERING_QUALITY_BETTER", VG_RENDERING_QUALITY_BETTER)
        .export_values();

    py::enum_<VGPixelLayout>(m, "VGPixelLayout")
        .value("VG_PIXEL_LAYOUT_UNKNOWN", VG_PIXEL_LAYOUT_UNKNOWN)
        .value("VG_PIXEL_LAYOUT_RGB_VERTICAL", VG_PIXEL_LAYOUT_RGB_VERTICAL)
        .value("VG_PIXEL_LAYOUT_BGR_VERTICAL", VG_PIXEL_LAYOUT_BGR_VERTICAL)
        .value("VG_PIXEL_LAYOUT_RGB_HORIZONTAL", VG_PIXEL_LAYOUT_RGB_HORIZONTAL)
        .value("VG_PIXEL_LAYOUT_BGR_HORIZONTAL", VG_PIXEL_LAYOUT_BGR_HORIZONTAL)
        .export_values();

    py::enum_<VGMatrixMode>(m, "VGMatrixMode")
        .value("VG_MATRIX_PATH_USER_TO_SURFACE", VG_MATRIX_PATH_USER_TO_SURFACE)
        .value("VG_MATRIX_IMAGE_USER_TO_SURFACE",
               VG_MATRIX_IMAGE_USER_TO_SURFACE)
        .value("VG_MATRIX_FILL_PAINT_TO_USER", VG_MATRIX_FILL_PAINT_TO_USER)
        .value("VG_MATRIX_STROKE_PAINT_TO_USER", VG_MATRIX_STROKE_PAINT_TO_USER)
        .value("VG_MATRIX_GLYPH_USER_TO_SURFACE",
               VG_MATRIX_GLYPH_USER_TO_SURFACE)
        .export_values();

    py::enum_<VGMaskOperation>(m, "VGMaskOperation")
        .value("VG_CLEAR_MASK", VG_CLEAR_MASK)
        .value("VG_FILL_MASK", VG_FILL_MASK)
        .value("VG_SET_MASK", VG_SET_MASK)
        .value("VG_UNION_MASK", VG_UNION_MASK)
        .value("VG_INTERSECT_MASK", VG_INTERSECT_MASK)
        .value("VG_SUBTRACT_MASK", VG_SUBTRACT_MASK)
        .export_values();

    py::enum_<VGPaintParamType>(m, "VGPaintParamType")
        .value("VG_PAINT_TYPE", VG_PAINT_TYPE)
        .value("VG_PAINT_COLOR", VG_PAINT_COLOR)
        .value("VG_PAINT_COLOR_RAMP_SPREAD_MODE",
               VG_PAINT_COLOR_RAMP_SPREAD_MODE)
        .value("VG_PAINT_COLOR_RAMP_PREMULTIPLIED",
               VG_PAINT_COLOR_RAMP_PREMULTIPLIED)
        .value("VG_PAINT_COLOR_RAMP_STOPS", VG_PAINT_COLOR_RAMP_STOPS)
        .value("VG_PAINT_LINEAR_GRADIENT", VG_PAINT_LINEAR_GRADIENT)
        .value("VG_PAINT_RADIAL_GRADIENT", VG_PAINT_RADIAL_GRADIENT)
        .value("VG_PAINT_PATTERN_TILING_MODE", VG_PAINT_PATTERN_TILING_MODE)
        .value("VG_PAINT_2x3_GRADIENT", VG_PAINT_2x3_GRADIENT)
        .export_values();

    py::enum_<VGPaintMode>(m, "VGPaintMode")
        .value("VG_STROKE_PATH", VG_STROKE_PATH)
        .value("VG_FILL_PATH", VG_FILL_PATH)
        .export_values()
        .def("__or__", [](VGPaintMode a, VGPaintMode b) {
            return static_cast<VGPaintMode>(static_cast<int>(a) |
                                            static_cast<int>(b));
        });

    py::enum_<VGPathDatatype>(m, "VGPathDatatype")
        .value("VG_PATH_DATATYPE_S_8", VG_PATH_DATATYPE_S_8)
        .value("VG_PATH_DATATYPE_S_16", VG_PATH_DATATYPE_S_16)
        .value("VG_PATH_DATATYPE_S_32", VG_PATH_DATATYPE_S_32)
        .value("VG_PATH_DATATYPE_F", VG_PATH_DATATYPE_F)
        .export_values();

    py::enum_<VGPathCapabilities>(m, "VGPathCapabilities")
        .value("VG_PATH_CAPABILITY_APPEND_FROM", VG_PATH_CAPABILITY_APPEND_FROM)
        .value("VG_PATH_CAPABILITY_APPEND_TO", VG_PATH_CAPABILITY_APPEND_TO)
        .value("VG_PATH_CAPABILITY_MODIFY", VG_PATH_CAPABILITY_MODIFY)
        .value("VG_PATH_CAPABILITY_TRANSFORM_FROM",
               VG_PATH_CAPABILITY_TRANSFORM_FROM)
        .value("VG_PATH_CAPABILITY_TRANSFORM_TO",
               VG_PATH_CAPABILITY_TRANSFORM_TO)
        .value("VG_PATH_CAPABILITY_INTERPOLATE_FROM",
               VG_PATH_CAPABILITY_INTERPOLATE_FROM)
        .value("VG_PATH_CAPABILITY_INTERPOLATE_TO",
               VG_PATH_CAPABILITY_INTERPOLATE_TO)
        .value("VG_PATH_CAPABILITY_PATH_LENGTH", VG_PATH_CAPABILITY_PATH_LENGTH)
        .value("VG_PATH_CAPABILITY_POINT_ALONG_PATH",
               VG_PATH_CAPABILITY_POINT_ALONG_PATH)
        .value("VG_PATH_CAPABILITY_TANGENT_ALONG_PATH",
               VG_PATH_CAPABILITY_TANGENT_ALONG_PATH)
        .value("VG_PATH_CAPABILITY_PATH_BOUNDS", VG_PATH_CAPABILITY_PATH_BOUNDS)
        .value("VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS",
               VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS)
        .value("VG_PATH_CAPABILITY_ALL", VG_PATH_CAPABILITY_ALL)
        .export_values();

    py::enum_<VGImageFormat>(m, "VGImageFormat")
        .value("VG_sRGBX_8888", VG_sRGBX_8888)
        .value("VG_sRGBA_8888", VG_sRGBA_8888)
        .value("VG_sRGBA_8888_PRE", VG_sRGBA_8888_PRE)
        .value("VG_sRGB_565", VG_sRGB_565)
        .value("VG_sRGBA_5551", VG_sRGBA_5551)
        .value("VG_sRGBA_4444", VG_sRGBA_4444)
        .value("VG_sL_8", VG_sL_8)
        .value("VG_lRGBX_8888", VG_lRGBX_8888)
        .value("VG_lRGBA_8888", VG_lRGBA_8888)
        .value("VG_lRGBA_8888_PRE", VG_lRGBA_8888_PRE)
        .value("VG_lL_8", VG_lL_8)
        .value("VG_A_8", VG_A_8)
        .value("VG_BW_1", VG_BW_1)
        .value("VG_A_1", VG_A_1)
        .value("VG_A_4", VG_A_4)
        .value("VG_sXRGB_8888", VG_sXRGB_8888)
        .value("VG_sARGB_8888", VG_sARGB_8888)
        .value("VG_sARGB_8888_PRE", VG_sARGB_8888_PRE)
        .value("VG_sARGB_1555", VG_sARGB_1555)
        .value("VG_sARGB_4444", VG_sARGB_4444)
        .value("VG_lXRGB_8888", VG_lXRGB_8888)
        .value("VG_lARGB_8888", VG_lARGB_8888)
        .value("VG_lARGB_8888_PRE", VG_lARGB_8888_PRE)
        .value("VG_sBGRX_8888", VG_sBGRX_8888)
        .value("VG_sBGRA_8888", VG_sBGRA_8888)
        .value("VG_sBGRA_8888_PRE", VG_sBGRA_8888_PRE)
        .value("VG_sBGR_565", VG_sBGR_565)
        .value("VG_sBGRA_5551", VG_sBGRA_5551)
        .value("VG_sBGRA_4444", VG_sBGRA_4444)
        .value("VG_lBGRX_8888", VG_lBGRX_8888)
        .value("VG_lBGRA_8888", VG_lBGRA_8888)
        .value("VG_lBGRA_8888_PRE", VG_lBGRA_8888_PRE)
        .value("VG_sXBGR_8888", VG_sXBGR_8888)
        .value("VG_sABGR_8888", VG_sABGR_8888)
        .value("VG_sABGR_8888_PRE", VG_sABGR_8888_PRE)
        .value("VG_sABGR_1555", VG_sABGR_1555)
        .value("VG_sABGR_4444", VG_sABGR_4444)
        .value("VG_lXBGR_8888", VG_lXBGR_8888)
        .value("VG_lABGR_8888", VG_lABGR_8888)
        .value("VG_lABGR_8888_PRE", VG_lABGR_8888_PRE)
        .export_values();

    py::enum_<VGImageQuality>(m, "VGImageQuality")
        .value("VG_IMAGE_QUALITY_NONANTIALIASED",
               VG_IMAGE_QUALITY_NONANTIALIASED)
        .value("VG_IMAGE_QUALITY_FASTER", VG_IMAGE_QUALITY_FASTER)
        .value("VG_IMAGE_QUALITY_BETTER", VG_IMAGE_QUALITY_BETTER)
        .export_values();

    // #define VG_PATH_FORMAT_STANDARD 0
    m.attr("VG_PATH_FORMAT_STANDARD") = VG_PATH_FORMAT_STANDARD;

    /// Functions
    m.def("vgCreateContextMNK", &vgCreateContextMNK, "Create MonkVG context");

    // paint
    m.def("vgCreatePaint", &vgCreatePaint, "Create a paint");
    m.def("vgDestroyPaint", &vgDestroyPaint, "Destroy a paint");
    m.def("vgSetPaint", &vgSetPaint, "Set a paint");

    // parameters
    m.def("vgSetParameterf", &vgSetParameterf, "Set a parameter as a float");
    m.def("vgSetParameteri", &vgSetParameteri, "Set a parameter as an integer");
    m.def(
        "vgSetParameterfv",
        [](VGHandle object, VGint paramType, py::list values) {
            std::vector<VGfloat> v;
            for (auto value : values) {
                v.push_back(value.cast<VGfloat>());
            }
            vgSetParameterfv(object, paramType, v.size(), v.data());
        },
        "Set a parameter as a float vector");
    m.def("vgSeti", &vgSeti, "Set an integer parameter");
    m.def("vgSetf", &vgSetf, "Set a float parameter");

    // path
    m.def("vgCreatePath", &vgCreatePath, "Create a path");
    m.def("vgDestroyPath", &vgDestroyPath, "Destroy a path");
    m.def("vgDrawPath", &vgDrawPath, "Draw a path");

    // matrix
    m.def("vgLoadIdentity", &vgLoadIdentity, "Load the identity matrix");
    m.def("vgTranslate", &vgTranslate, "Translate the matrix");
    m.def("vgScale", &vgScale, "Scale the matrix");
    m.def("vgRotate", &vgRotate, "Rotate the matrix");
    m.def("vgShear", &vgShear, "Shear the matrix");
    m.def(
        "vgLoadMatrix",
        [](py::list m) {
            std::vector<VGfloat> v;
            for (auto value : m) {
                v.push_back(value.cast<VGfloat>());
            }
            vgLoadMatrix(v.data());
        },
        "Load the matrix");
    m.def(
        "vgGetMatrix",
        [](py::list &m) {
            std::vector<VGfloat> v(9);
            vgGetMatrix(v.data());
            for (int i = 0; i < 9; i++) {
                m.append(v[i]);
            }
        },
        "Get the matrix");
    m.def(
        "vgMultMatrix",
        [](py::list m) {
            std::vector<VGfloat> v;
            for (auto value : m) {
                v.push_back(value.cast<VGfloat>());
            }
            vgMultMatrix(v.data());
        },
        "Multiply the matrix");

    // camera
    m.def("vgPushOrthoCamera", &vgPushOrthoCamera,
          "Push an orthographic camera onto the matrix stack");
    m.def("vgPopOrthoCamera", &vgPopOrthoCamera,
          "Pop the orthographic camera off the matrix stack");

    // image
    m.def("vgCreateImage", &vgCreateImage, "Create an image");
    m.def(
        "vgImageSubData",
        [](VGImage image, py::buffer data, VGint dataStride,
           VGImageFormat dataFormat, VGint x, VGint y, VGint width,
           VGint height) {
            py::buffer_info info = data.request();
            vgImageSubData(image, info.ptr, dataStride, dataFormat, x, y, width,
                           height);
        },
        "Copy the image data to the OpenVG image");
    m.def("vgDestroyImage", &vgDestroyImage, "Destroy an image");
    m.def("vgDrawImage", &vgDrawImage, "Draw an image");
    m.def("vgChildImage", &vgChildImage, "Create a child image");

    /// vgu
    py::enum_<VGUErrorCode>(m, "VGUErrorCode")
        .value("VGU_NO_ERROR", VGU_NO_ERROR)
        .value("VGU_BAD_HANDLE_ERROR", VGU_BAD_HANDLE_ERROR)
        .value("VGU_ILLEGAL_ARGUMENT_ERROR", VGU_ILLEGAL_ARGUMENT_ERROR)
        .value("VGU_OUT_OF_MEMORY_ERROR", VGU_OUT_OF_MEMORY_ERROR)
        .value("VGU_PATH_CAPABILITY_ERROR", VGU_PATH_CAPABILITY_ERROR)
        .value("VGU_BAD_WARP_ERROR", VGU_BAD_WARP_ERROR)
        .export_values();

    m.def("vguRect", &vguRect, "Create a rectangle path");
}
