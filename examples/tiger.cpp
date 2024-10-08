// MonkVG OpenVG interface
#include <MonkVG/openvg.h>
#include <MonkVG/vgext.h>

// OpenGL window creation libraries
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#else
#define GLFW_INCLUDE_ES32
#include <GLFW/glfw3.h>
#endif 
#include <glm/glm.hpp>

// System
#include <iostream>

// Tiger Paths
#include "tiger_paths.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

VGPath *tigerPaths = NULL;
VGPaint tigerStroke;
VGPaint tigerFill;
VGfloat sx = 0.5f, sy = 0.5f;
VGfloat tx = 1.0f, ty = 1.0f;
VGfloat ang = 0.0f;
int     animate = 1;

void displayTiger(float interval) {
    int            i;
    const VGfloat *style;
    VGfloat        clearColor[] = {1, 1, 1, 1};

    if (animate) {
        ang += interval * 360 * 0.1f;
        if (ang > 360)
            ang -= 360;
    }

    vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
    vgClear(0, 0, WINDOW_HEIGHT, WINDOW_WIDTH);

    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgLoadIdentity();
    vgScale(sx, sy);
    vgRotate(ang);
    vgTranslate(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    for (i = 0; i < pathCount; ++i) {

        style = styleArrays[i];
        vgSetParameterfv(tigerStroke, VG_PAINT_COLOR, 4, &style[0]);
        vgSetParameterfv(tigerFill, VG_PAINT_COLOR, 4, &style[4]);
        vgSetf(VG_STROKE_LINE_WIDTH, style[8]);
        vgDrawPath(tigerPaths[i], (VGint)style[9]);
    }
}

void loadTiger() {
    int    i;
    VGPath temp;

    temp = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1, 0, 0, 0,
                        VG_PATH_CAPABILITY_ALL);
    tigerPaths = (VGPath *)malloc(pathCount * sizeof(VGPath));
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);

    for (i = 0; i < pathCount; ++i) {

        vgClearPath(temp, VG_PATH_CAPABILITY_ALL);
        vgAppendPathData(temp, commandCounts[i], commandArrays[i],
                         dataArrays[i]);

        tigerPaths[i] =
            vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1, 0, 0,
                         0, VG_PATH_CAPABILITY_ALL);
        vgTransformPath(tigerPaths[i], temp);
    }

    tigerStroke = vgCreatePaint();
    tigerFill = vgCreatePaint();
    vgSetPaint(tigerStroke, VG_STROKE_PATH);
    vgSetPaint(tigerFill, VG_FILL_PATH);
    vgLoadIdentity();
    vgDestroyPath(temp);
}

void cleanupTiger() { free(tigerPaths); }

int main(int argc, char **argv) {
    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // create OpenGL window
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

    loadTiger();

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


        displayTiger(0.01f);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // destroy MonkVG
    cleanupTiger();
    vgDestroyContextMNK();

    glfwDestroyWindow(window);

    return 0;
}
