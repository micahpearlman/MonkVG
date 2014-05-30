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
#include "mkMath.h"
#include "glPlatform.h"

#include <vector>

namespace MonkVG {
	class OpenGLImage;

	class OpenGLBatch : public IBatch {
	public:
	
		OpenGLBatch();
		virtual ~OpenGLBatch();
		
		virtual void draw();
		virtual void dump( void **vertices, size_t *size );
		virtual void finalize();
		
		void add( const GLfloat* v, size_t n, const VGfloat* color);
		void tadd( const GLfloat* v, size_t n, const OpenGLImage* tex, 
			   float x, float y, float w, float h);
		
	private:

		struct vertex_t {
			GLfloat x, y;
		};
		
		std::vector<vertex_t> _vertices;

		struct tvertex_t {
			GLfloat x, y;
			GLfloat u, v;
		};
		std::vector<tvertex_t> _tvertices;

		struct drawop_t {
			GLfloat matrix[16];
			GLfloat color[4];
			unsigned from;
			unsigned to;
		};
		struct tdrawop_t {
			GLfloat matrix[16];
			const OpenGLImage* tex;
			unsigned from;
			unsigned to;
		};
		std::vector<drawop_t> _drawops;
		std::vector<tdrawop_t> _tdrawops;

		GLuint _vbo;
		GLuint _tvbo;


		void drawOp(const drawop_t & op) const;
		void tdrawOp(const tdrawop_t & op) const;
		static void fillMatrix(GLfloat *m);

		void drawColored() const;
		void drawTextured() const;

	};
}

#endif // __glBatch_h__
