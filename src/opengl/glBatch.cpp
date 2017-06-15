//
//  glBatch.cpp
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 6/27/11.
//  Copyright 2011 Zero Vision. All rights reserved.
//

#include <cstring> // for std::memcpy
#include "glBatch.h"
#include "glImage.h"
#include "glContext.h"

namespace MonkVG {
	
	OpenGLBatch::OpenGLBatch()
	:	IBatch()
	,	_vbo(-1)
	,	_tvbo(-1)
	{}
	OpenGLBatch::~OpenGLBatch() {
		if ( _vbo != -1 )
			GL->glDeleteBuffers( 1, &_vbo );
		if ( _tvbo != -1 )
			GL->glDeleteBuffers( 1, &_tvbo );
	}

	void OpenGLBatch::fillMatrix(GLfloat * dm) {
		const Matrix33 & m = *IContext::instance().getActiveMatrix();

		dm[0] = m.m[0];
		dm[1] = m.m[3];
		dm[2] = 0;
		dm[3] = 0;

		dm[4] = m.m[1];
		dm[5] = m.m[4];
		dm[6] = 0;
		dm[7] = 0;

		dm[8] = 0;
		dm[9] = 0;
		dm[10] = 1;
		dm[11] = 0;

		dm[12] = m.m[2];
		dm[13] = m.m[5];
		dm[14] = 0;
		dm[15] = 1;
	}

	void OpenGLBatch::tadd( const GLfloat* verts, size_t n, const OpenGLImage* tex, 
			       float x, float y, float w, float h) {
		tdrawop_t op;
		op.tex = tex;
		fillMatrix(op.matrix);
		op.from = (unsigned)_tvertices.size();
		float xs = 1.0f / w;
		float ys = 1.0f / h;
		for (size_t i = 0; i < n / 2; i++) {
			tvertex_t v;
			v.x = verts[2*i+0];
			v.y = verts[2*i+1];
			v.u = fabsf(v.x - x) * xs;
			v.v = fabsf(v.y - y) * ys;
			_tvertices.push_back(v);
		}
		op.to = (unsigned)_tvertices.size();
		_tdrawops.push_back(op);
	}

	void OpenGLBatch::add( const GLfloat* verts, size_t n, const VGfloat * fc) {
		drawop_t op;
		std::memcpy(op.color, fc, sizeof(op.color));
		fillMatrix(op.matrix);
		op.from = (unsigned)_vertices.size();
		for (size_t i = 0; i < n / 2; i++) {
			vertex_t v;
			v.x = verts[2*i+0];
			v.y = verts[2*i+1];
			_vertices.push_back(v);
		}
		op.to = (unsigned)_vertices.size();
		_drawops.push_back(op);
	}
	
	void OpenGLBatch::finalize() {
		assert(_vbo == -1);
		GL->glGenBuffers( 1, &_vbo );
		GL->glBindBuffer( GL_ARRAY_BUFFER, _vbo );
		GL->glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.data(), GL_STATIC_DRAW );
		_vertices.clear();

		assert(_tvbo == -1);
		GL->glGenBuffers( 1, &_tvbo );
		GL->glBindBuffer( GL_ARRAY_BUFFER, _tvbo );
		GL->glBufferData( GL_ARRAY_BUFFER, _tvertices.size() * sizeof(_tvertices[0]), _tvertices.data(), GL_STATIC_DRAW );
		_tvertices.clear();
		
	}
    
	void OpenGLBatch::dump( void **vertices, size_t *size ) {
        
		*size = _vertices.size() * sizeof( GLfloat );
		*vertices = malloc( *size );
		
		std::memcpy( *vertices, _vertices.data(), *size );
		
	}

	void OpenGLBatch::tdrawOp(const tdrawop_t & op) const {
		op.tex->bind();
		GL->glLoadMatrixf( op.matrix );
		GL->glDrawArrays( GL_TRIANGLES, op.from, GLsizei(op.to - op.from) );
		OpenGLImage::unbind();
	}

	void OpenGLBatch::drawOp(const drawop_t & op) const {
		GL->glColor4f( op.color[0], op.color[1], op.color[2], op.color[3] );
		GL->glLoadMatrixf( op.matrix );
		GL->glDrawArrays( GL_TRIANGLES, op.from, GLsizei(op.to - op.from) );
	}

	void OpenGLBatch::drawColored() const {
//		GL->glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//		GL->glDisableClientState( GL_COLOR_ARRAY );

		GL->glBindBuffer( GL_ARRAY_BUFFER, _vbo );
		GL->glVertexPointer( 2, GL_FLOAT, 0, 0 );
		size_t n = _drawops.size();
		for (size_t i = 0; i < n; i++) {
			drawOp(_drawops[i]);
		}
	}

	void OpenGLBatch::drawTextured() const {
		GL->glColor4f( 1, 1, 1, 1 );
		GL->glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		GL->glBindBuffer( GL_ARRAY_BUFFER, _tvbo );
		GL->glVertexPointer( 2, GL_FLOAT, sizeof(tvertex_t), 0 );
		GL->glTexCoordPointer( 2, GL_FLOAT, sizeof(tvertex_t), (GLvoid*)8 );
		size_t n = _tdrawops.size();
		for (size_t i = 0; i < n; i++) {
			tdrawOp(_tdrawops[i]);
		}
		GL->glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}
	
	void OpenGLBatch::draw() {
		OpenGLContext& glContext = (MonkVG::OpenGLContext&)IContext::instance();
		glContext.beginRender();
		GL->glPushMatrix();

		GL->glEnableClientState( GL_VERTEX_ARRAY );

		drawTextured();

		drawColored();


		GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );


		GL->glPopMatrix();
		glContext.endRender();
	}
}
