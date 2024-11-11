import sys
import os

# add the path to the monkvg python bindings to the system path
# this assumes that the python bindings are built in the build directory
# if you are using this as a template, you will need to change the path to the python bindings
sys.path.append(
    os.path.join(os.path.dirname(__file__), '../build/debug/bindings'))
print(sys.path)

# assumes we are using the automatically generated venv in the build directory

import monkvg_py as vg
import pygame
from pygame.locals import *
import OpenGL.GL as gl
import OpenGL.GLU as glu
from PIL import Image

# its alive test
print(vg.add(1, 2))  # 3


def main():
    pygame.init()

    # Request OpenGL 3.2 context
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 2)

    # if macos
    if sys.platform == "darwin":
        pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK,
                                        pygame.GL_CONTEXT_PROFILE_CORE)

    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    # create monkvg context
    vg.vgCreateContextMNK(display[0], display[1],
                          vg.VG_RENDERING_BACKEND_TYPE_OPENGL33)

    # create fill and stroke paints
    fill = vg.vgCreatePaint()
    vg.vgSetPaint(fill, vg.VG_FILL_PATH)
    vg.vgSetParameterfv(fill, vg.VG_PAINT_COLOR, [1, 0, 0, 1])

    stroke = vg.vgCreatePaint()
    vg.vgSetPaint(stroke, vg.VG_STROKE_PATH)
    vg.vgSetParameterfv(stroke, vg.VG_PAINT_COLOR, [0, 1, 0, 1])

    # create a path
    path = vg.vgCreatePath(vg.VG_PATH_FORMAT_STANDARD, vg.VG_PATH_DATATYPE_F,
                           1, 0, 0, 0, vg.VG_PATH_CAPABILITY_ALL)
    vg.vguRect(path, 0, 0, 200, 200)

    # load an image
    fimage = Image.open("assets/roy.png")
    image_data = fimage.convert("RGBA").tobytes()

    # create an image
    image = vg.vgCreateImage(vg.VG_sRGBA_8888, fimage.size[0], fimage.size[1],
                             vg.VG_IMAGE_QUALITY_BETTER)
    vg.vgImageSubData(image, image_data, fimage.size[0] * 4, vg.VG_sRGBA_8888,
                        0, 0, fimage.size[0], fimage.size[1])
    
    # create a child image
    child_img = vg.vgChildImage(image, 0, 0, fimage.size[0]//2, fimage.size[1]//2)
    

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        gl.glClearColor(0.5, 0.5, 0.5, 1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)

        # do an orthographic projection
        vg.vgPushOrthoCamera(0.0, display[0], 0.0, display[1], -1.0, 1.0)

        # draw the path
        vg.vgSeti(vg.VG_MATRIX_MODE, vg.VG_MATRIX_PATH_USER_TO_SURFACE)
        vg.vgLoadIdentity()
        vg.vgTranslate(display[0] / 2, display[1] / 2)
        vg.vgSetf(vg.VG_STROKE_LINE_WIDTH, 5.0)
        vg.vgSetPaint(fill, vg.VG_FILL_PATH)
        vg.vgSetPaint(stroke, vg.VG_STROKE_PATH)
        vg.vgDrawPath(path, vg.VG_FILL_PATH | vg.VG_STROKE_PATH)
        
        # draw the image
        vg.vgSeti(vg.VG_MATRIX_MODE, vg.VG_MATRIX_IMAGE_USER_TO_SURFACE)
        vg.vgLoadIdentity()
        vg.vgScale(0.25, 0.25)
        vg.vgTranslate(50, 50)
        vg.vgDrawImage(image)
        
        # draw the child image
        vg.vgSeti(vg.VG_MATRIX_MODE, vg.VG_MATRIX_IMAGE_USER_TO_SURFACE)
        vg.vgLoadIdentity()
        vg.vgScale(0.25, 0.25)
        vg.vgTranslate(50, 200)
        vg.vgDrawImage(child_img)

        vg.vgPopOrthoCamera()
        pygame.display.flip()
        pygame.time.wait(10)


if __name__ == "__main__":
    main()
