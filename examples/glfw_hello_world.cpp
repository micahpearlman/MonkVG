// MonkVG OpenVG interface
#include <MonkVG/openvg.h>
#include <MonkVG/vgext.h>

// OpenGL window creation libraries
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#else
#define GLFW_INCLUDE_ES32
#include <GLFW/glfw3.h>
#endif

// GLM math library
#include <glm/glm.hpp>

// STB image loader
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// System
#include <iostream>

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768

int main(int argc, char **argv) {
    std::cout << "Hello, MonkVG!\n";
    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // create OpeGL window
#if defined(__APPLE__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_ANY_PROFILE); // We don't want the old OpenGL
#endif

    // Open a window and create its OpenGL context
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "MonkVG Hello World", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    vgCreateContextMNK(WINDOW_WIDTH, WINDOW_HEIGHT,
                       VG_RENDERING_BACKEND_TYPE_OPENGL33);

    // create fill and stroke paints
    VGPaint fill_paint = vgCreatePaint();
    vgSetPaint(fill_paint, VG_FILL_PATH);
    VGfloat fill_color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    vgSetParameterfv(fill_paint, VG_PAINT_COLOR, 4, &fill_color[0]);

    VGPaint stroke_paint = vgCreatePaint();
    vgSetPaint(stroke_paint, VG_STROKE_PATH);
    VGfloat stroke_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    vgSetParameterfv(stroke_paint, VG_PAINT_COLOR, 4, &stroke_color[0]);

    // create a simple box path
    VGPath path;
    path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1, 0, 0, 0,
                        VG_PATH_CAPABILITY_ALL);
    vguRect(path, 0.0f, 0.0f, 100.0f, 150.0f);

    // load and create an opencv image
    int img_width, img_height, img_channels;

    // Load the image (JPEG, PNG, etc.)
    const char    *filename = "assets/roy.png"; // Replace with your image path
    unsigned char *img_data =
        stbi_load(filename, &img_width, &img_height, &img_channels, 0);

    if (img_data == nullptr) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return -1;
    }

    // Display image info
    std::cout << "Loaded image: " << filename << std::endl;
    std::cout << "Width: " << img_width << ", Height: " << img_height
              << ", Channels: " << img_channels << std::endl;
    assert(img_channels == 4);
    // Create an OpenVG image with the appropriate format
    VGImage vg_image = vgCreateImage(VG_sRGBA_8888, img_width, img_height,
                                     VG_IMAGE_QUALITY_BETTER);

    // Copy the image data to the OpenVG image
    vgImageSubData(vg_image, img_data, img_width * 4, VG_sRGBA_8888, 0, 0,
                   img_width, img_height);

    // Free image memory
    stbi_image_free(img_data);

    // create a child image
    VGImage child_image =
        vgChildImage(vg_image, 0, 0, img_width / 2, img_height / 2);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // set viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    do {

        // Clear the screen.
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// do an ortho camera
        // NOTE:  this is not standard OpenVG
        // NOTE: Bottom left is 0,0
        vgPushOrthoCamera(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

        /// draw the basic path
        // set up path trasnform
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
        vgLoadIdentity();
        vgTranslate(width / 2, height / 2);

        // stroke wideth
        vgSetf(VG_STROKE_LINE_WIDTH, 5.0f);

        // fill and stroke paints
        vgSetPaint(fill_paint, VG_FILL_PATH);
        vgSetPaint(stroke_paint, VG_STROKE_PATH);

        // draw the path with fill and stroke
        vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);

        // draw the image
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        vgLoadIdentity();
        vgScale(0.25f, 0.25f);
        vgTranslate(50, 50);
        vgDrawImage(vg_image);

        // draw the child image
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        vgLoadIdentity();
        vgScale(0.25f, 0.25f);
        vgTranslate(50, 200);
        vgDrawImage(child_image);

        // pop the ortho camera
        vgPopOrthoCamera();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // destroy MonkVG
    vgDestroyPath(path);
    vgDestroyPaint(fill_paint);
    vgDestroyPaint(stroke_paint);
    vgDestroyImage(child_image);
    vgDestroyImage(vg_image);
    vgDestroyContextMNK();

    glfwDestroyWindow(window);

    return 0;
}
