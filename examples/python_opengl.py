import sys
import os 



# add the path to the monkvg python bindings to the system path
# this assumes that the python bindings are built in the build directory
sys.path.append(os.path.join(os.path.dirname(__file__), '../build/debug/bindings'))
print(sys.path)


# assumes we are using the automatically generated venv in the build directory

import monkvg_py as vg
import pygame
from pygame.locals import *
import OpenGL.GL as gl
import OpenGL.GLU as glu

# its alive test
print(vg.add(1, 2)) # 3


def main():
    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)
    
    glu.gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
    gl.glTranslatef(0.0, 0.0, -5)
    
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
        
        gl.glRotatef(1, 3, 1, 1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)
        gl.glBegin(gl.GL_TRIANGLES)
        gl.glVertex3f(0, 1, 0)
        gl.glVertex3f(-1, -1, 0)
        gl.glVertex3f(1, -1, 0)
        gl.glEnd()
        
        # vg.draw()
        
        pygame.display.flip()
        pygame.time.wait(10)    
    

if __name__ == "__main__":
    main()