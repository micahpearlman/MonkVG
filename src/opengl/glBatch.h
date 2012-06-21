//
//  glBatch.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/27/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//
#ifndef __glBatch_h__
#define __glBatch_h__

#include "mkBatch.h"

#if defined(__APPLE__)
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #include "glu.h"
    #else
        #include <OpenGL/glu.h>
        #define glOrthof glOrtho
    #endif
#elif ANDROID
#include "glu.h"
#elif WIN32
#define WIN32_LEAN_AND_MEAN
#include <GL/glew.h>
#define glOrthof glOrthofOES
//#include <gl\gl.h>		// Header File For The OpenGL32 Library
//#include <gl\glu.h>		// Header File For The GLu32 Library
#elif __linux__
#define GL_GLEXT_PROTOTYPES 1
#include <sys/types.h>
#include <stdlib.h>
#include <GL/glew.h>
#define glOrthof glOrthofOES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif // #if defined(__APPLE__)

#include <vector>

namespace MonkVG {
	class OpenGLBatch : public IBatch {
	public:
	
		OpenGLBatch();
		virtual ~OpenGLBatch();
		
		virtual void draw();
		virtual void finalize();
		
		void addPathVertexData( GLfloat* fillVerts, size_t fillVertCnt, GLfloat* strokeVerts, size_t strokeVertCnt, VGbitfield paintModes );
		
	public:
		struct vertex_t {
			GLfloat		v[2];
			GLuint		color;
		};
	private:
		
		std::vector<vertex_t>	_vertices;
		size_t					_vertexCount;
		GLuint					_vbo;

		
	};
}

#endif // __glBatch_h__