// MonkVG OpenVG interface
#include <MonkVG/openvg.h>
#include <MonkVG/vgext.h>
#include <MonkVG/vgu.h>

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
#include <fstream>
#include <sstream>

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

    // open bitmap font file and read it into a string
    std::ifstream     bmp_fnt_file("arial.fnt");
    std::stringstream bmp_fnt_stream;
    bmp_fnt_stream << bmp_fnt_file.rdbuf();

    // open bitmap font imageı
    int            bmp_fnt_width = 0;
    int            bmp_fnt_height = 0;
    int            bmp_fnt_channels = 0;
    unsigned char *bmp_fnt_data = stbi_load(
        "arial.png", &bmp_fnt_width, &bmp_fnt_height, &bmp_fnt_channels, 0);
    if (bmp_fnt_data == nullptr) {
        std::cerr << "Failed to load image: arial.png" << std::endl;
        return -1;
    }
    // Display image info
    std::cout << "Width: " << bmp_fnt_width << ", Height: " << bmp_fnt_height
              << ", Channels: " << bmp_fnt_channels << std::endl;
    assert(bmp_fnt_channels == 4);

    VGImage bmp_fnt_image = vgCreateImage(
        VG_sRGBA_8888, bmp_fnt_width, bmp_fnt_height, VG_IMAGE_QUALITY_BETTER);
    // Copy the image data to the OpenVG image
    vgImageSubData(bmp_fnt_image, bmp_fnt_data, bmp_fnt_width * 4,
                   VG_sRGBA_8888, 0, 0, bmp_fnt_width, bmp_fnt_height);

    // Free image memory
    stbi_image_free(bmp_fnt_data);

    VGFont font =
        vgCreateFontFromBmFnt(bmp_fnt_stream.str().c_str(),
                               bmp_fnt_stream.str().size(), bmp_fnt_image);

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
        vgPushOrthoCamera(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

        /// draw the basic path
        // set up path trasnform
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
        vgLoadIdentity();
        vgTranslate(width / 2, height / 2);

        // pop the ortho camera
        vgPopOrthoCamera();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // destroy MonkVG
    vgDestroyContextMNK();

    glfwDestroyWindow(window);

    return 0;
}
