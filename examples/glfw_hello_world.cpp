// MonkVG OpenVG interface
#include <openvg.h>
#include <vgext.h>

// OpenGL window creation libraries
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#else
#define GLFW_INCLUDE_ES32
#include <GLFW/glfw3.h>
#endif

#include <glm/glm.hpp>

// System
#include <iostream>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int main(int argc, char **argv) {
    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // create OpeGL window
#if defined(__APPLE__)
    /* We need to explicitly ask for a 3.2 context on OS X */
    // see: https://gist.github.com/v3n/27e810ac744b076ceeb7
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
    GLFWwindow *window; // (In the accompanying source code, this variable is
                        // global for simplicity)
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "MonkVG Hello World",
                              NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW

    // Initialize MonkVG using GLES 2.0 rendering
    vgCreateContextMNK(WINDOW_WIDTH, WINDOW_HEIGHT,
                       VG_RENDERING_BACKEND_TYPE_OPENGLES20);

    // create a paint
    VGPaint paint;

    paint = vgCreatePaint();
    vgSetPaint(paint, VG_FILL_PATH);
    VGfloat color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    vgSetParameterfv(paint, VG_PAINT_COLOR, 4, &color[0]);

    // create a simple box path
    VGPath path;
    path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1, 0, 0, 0,
                        VG_PATH_CAPABILITY_ALL);
    vguRect(path, 0.0f, 0.0f, 100.0f, 150.0f);

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
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
        vgLoadIdentity();
        vgTranslate(width / 2, height / 2);
        vgSetPaint(paint, VG_FILL_PATH);
        vgDrawPath(path, VG_FILL_PATH);

        vgPopOrthoCamera();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // destroy MonkVG
    vgDestroyPath(path);
    vgDestroyPaint(paint);
    vgDestroyContextMNK();

    glfwDestroyWindow(window);

    return 0;
}
