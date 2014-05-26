/*
 *  mkvgContext.h
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkContext_h__
#define __mkContext_h__

#include "MonkVG/openvg.h"
#include "MonkVG/vgext.h"
#include "mkPath.h"
#include "mkPaint.h"
#include "mkImage.h"
#include "mkBatch.h"
#include "mkFont.h"
#include "mkMath.h"

namespace MonkVG {
	
	class IContext {
	public:
	
		IContext();
		
		// singleton instance
		static IContext& instance();
		virtual bool Initialize() = 0;
		virtual bool Terminate() = 0;
		
		//// factories ////
		virtual IPath* createPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities ) = 0;
		virtual void destroyPath( IPath* path ) = 0;
		virtual IPaint* createPaint() = 0;
		virtual void destroyPaint( IPaint* paint ) = 0;
		virtual IImage* createImage( VGImageFormat format,
									VGint width, VGint height,
									VGbitfield allowedQuality ) = 0;
		virtual void destroyImage( IImage* image ) = 0;
		virtual IBatch* createBatch() = 0;
		virtual void destroyBatch( IBatch* batch ) = 0;
		virtual IFont* createFont() = 0;
		virtual void destroyFont( IFont* font ) = 0;
		
		//// platform specific execution of stroke and fill ////
		virtual void stroke() = 0;
		virtual void fill() = 0;
		
		//// platform specific execution of Masking and Clearing ////
		virtual void clear(VGint x, VGint y, VGint width, VGint height) = 0;
		
		//// Paints ////
		virtual void setStrokePaint( IPaint* paint ) {
			_stroke_paint = paint;
		}
		inline IPaint* getStrokePaint() const {
			return _stroke_paint;
		}
		virtual void setFillPaint( IPaint* paint ) {
			_fill_paint = paint;
		}
		inline IPaint* getFillPaint() {
			return _fill_paint;
		}
		inline VGFillRule getFillRule() const {
			return _fill_rule;
		}
		inline void setFillRule( VGFillRule r ) {
			_fill_rule = r;
		}
		
		//// drawing context ////
		inline VGint getWidth() const {
			return _width;
		}
		inline void setWidth( VGint w ) {
			_width = w;
		}
		
		inline VGint getHeight() const {
			return _height;
		}
		inline void setHeight( VGint h ) {
			_height = h;
		}
		
		virtual void resize() = 0;
		
		//// parameters ////
		void set( VGuint type, VGfloat f );
		void set( VGuint type, const VGfloat * values );
		void set( VGuint type, VGint i );
		void get( VGuint type, VGfloat &f ) const;
		void get( VGuint type, VGfloat *fv ) const;
		void get( VGuint type, VGint &i ) const; 
		
		//// stroke parameters ////
		inline void setStrokeLineWidth( VGfloat w ) {
			_stroke_line_width = w;
		}
		inline VGfloat getStrokeLineWidth() const {
			return _stroke_line_width;
		}
		
		//// surface properties ////
		inline void setClearColor( const VGfloat *c ) {
			_clear_color[0] = c[0]; _clear_color[1] = c[1];
			_clear_color[2] = c[2]; _clear_color[3] = c[3];
		}
		inline void getClearColor( VGfloat *c ) const {
			c[0] = _clear_color[0]; c[1] = _clear_color[1];
			c[2] = _clear_color[2]; c[3] = _clear_color[3];
		}
		
		//// transforms ////
		inline Matrix33& getPathUserToSurface() {
			return _path_user_to_surface;
		}
		inline void setPathUserToSurface( const Matrix33& m ) {
			_path_user_to_surface = m;
		}
		virtual void setMatrixMode( VGMatrixMode mode ) {
			//			VG_MATRIX_PATH_USER_TO_SURFACE              = 0x1400,
			//			VG_MATRIX_IMAGE_USER_TO_SURFACE             = 0x1401,
			//			VG_MATRIX_FILL_PAINT_TO_USER                = 0x1402,
			//			VG_MATRIX_STROKE_PAINT_TO_USER              = 0x1403,
			//			VG_MATRIX_GLYPH_USER_TO_SURFACE             = 0x1404,
			_matrixMode = mode;
			switch (mode) {
				case VG_MATRIX_PATH_USER_TO_SURFACE:
					_active_matrix = &_path_user_to_surface;
					break;
				case VG_MATRIX_IMAGE_USER_TO_SURFACE:
					_active_matrix = &_image_user_to_surface;
					break;
				case VG_MATRIX_GLYPH_USER_TO_SURFACE:
					_active_matrix = &_glyph_user_to_surface;
					break;
				default:
					SetError(VG_ILLEGAL_ARGUMENT_ERROR);
					break;
			}
		}
		inline VGMatrixMode getMatrixMode() const { return _matrixMode; }
		inline Matrix33* getActiveMatrix() {
			return _active_matrix;
		}
		
		virtual void setIdentity() = 0;
		virtual void transform( VGfloat* t ) = 0; 
		virtual void scale( VGfloat sx, VGfloat sy ) = 0;
		virtual void translate( VGfloat x, VGfloat y ) = 0;
		virtual void rotate( VGfloat angle ) = 0;
		virtual void setTransform( const VGfloat* t ) = 0;
		virtual void multiply( const VGfloat* t ) = 0;
		
		//// error handling ////
		inline VGErrorCode getError() const { 
			return _error; 
		}
		inline void setError( const VGErrorCode e ) {
			_error = e;
		}
		
		/// rendering quality
		inline VGRenderingQuality getRenderingQuality() const { return _renderingQuality; }
		inline void setRenderingQuality( VGRenderingQuality rc ) { _renderingQuality = rc; }
		
		inline int32_t getTessellationIterations() const { return _tessellationIterations; }
		inline void setTessellationIterations( int32_t i ) { _tessellationIterations = i; }
		
		/// batch drawing
		virtual void startBatch( IBatch* batch ) = 0;
        virtual void dumpBatch( IBatch* batch, void **vertices, size_t *size ) = 0;
		virtual void endBatch( IBatch* batch ) = 0;
		IBatch* currentBatch() { return _currentBatch; }
		
		/// font
		void setGlyphOrigin( const VGfloat o[2] ) { _glyph_origin[0] = o[0]; _glyph_origin[1] = o[1]; }
		void getGlyphOrigin( VGfloat o[2] ) const { o[0] = _glyph_origin[0]; o[1] = _glyph_origin[1]; }
		
		/// image
		virtual VGImageMode getImageMode() const { return _imageMode; }
		virtual void setImageMode( VGImageMode im ) { _imageMode = im; }
        
        /// renderer
        virtual VGRenderingBackendTypeMNK getRenderingBackendType() const { return _backendRenderer; }
        virtual void setRenderingBackendType( VGRenderingBackendTypeMNK backendRenderer ) { _backendRenderer = backendRenderer; }
	
	protected:
	
		// surface properties
		VGint				_width, _height;
		VGfloat				_clear_color[4];
		
		// matrix transforms
		Matrix33			_path_user_to_surface;
		Matrix33			_image_user_to_surface;
		Matrix33			_glyph_user_to_surface;
		Matrix33			*_active_matrix;
		VGMatrixMode		_matrixMode;
		
		// stroke properties
		VGfloat				_stroke_line_width;			// VG_STROKE_LINE_WIDTH
		
		// rendering quality
		VGRenderingQuality	_renderingQuality;
		int32_t				_tessellationIterations;

		// paints
		IPaint*				_stroke_paint;
		IPaint*				_fill_paint;
		VGFillRule			_fill_rule;
		
		// font
		VGfloat				_glyph_origin[2];
		
		// batch
		IBatch*				_currentBatch;
		
		// imFW
		VGImageMode			_imageMode;

		// error 
		VGErrorCode			_error;
        
        // renderer
        VGRenderingBackendTypeMNK   _backendRenderer;
	};
}

#endif // __mkContext_h__
