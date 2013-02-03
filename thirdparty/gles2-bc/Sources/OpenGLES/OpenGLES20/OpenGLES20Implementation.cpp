/*
 Copyright 2009 Johannes Vuorinen
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at 
 
 http://www.apache.org/licenses/LICENSE-2.0 
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "OpenGLES20Implementation.h"
//#include <OpenGLES/ES2/gl.h>
//#include <OpenGLES/ES2/glext.h>
#define INCLUDE_ES1_HEADERS 0
#include "OpenGLESPlatform.h"
#include <stdlib.h>
#include "OpenGLESMath.h"

using namespace OpenGLES::OpenGLES2;

OpenGLES20Implementation::OpenGLES20Implementation() : OpenGLESImplementation()
{
	
}

OpenGLES20Implementation::~OpenGLES20Implementation()
{
	
}

void OpenGLES20Implementation::init() 
{
#if PLATFORM_IS_GLES 
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &colorReadFormat);
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &colorReadType);
#endif
    
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits);
	maxCombinedTextureImageUnits = std::min(3, maxCombinedTextureImageUnits); // TODO: currently shaders support 3 textures
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize);
#if PLATFORM_IS_GLES   
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentUniformVectors);
#endif
	
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);
	maxTextureImageUnits = std::min(3, maxTextureImageUnits); // TODO: currently shaders support 3 textures
#if PLATFORM_IS_GLES  
	glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
#endif
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureImageUnits);
#if PLATFORM_IS_GLES    
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformVectors);
#endif
    
	
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewportDims);
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numCompressedTextureFormats);
#if PLATFORM_IS_GLES    
	glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &numShaderBinaryFormats);
#endif
	shaderBinaryFormats = (int *)malloc(sizeof(int) * numShaderBinaryFormats);
#if PLATFORM_IS_GLES    
	glGetIntegerv(GL_SHADER_BINARY_FORMATS, shaderBinaryFormats);
#endif
	
#if PLATFORM_IS_GLES    
	unsigned char tmp;
	glGetBooleanv(GL_SHADER_COMPILER, &tmp);
	shaderCompilerSupported = tmp == 0;
#else
    shaderCompilerSupported = 1;
#endif
	
	glGetIntegerv(GL_DEPTH_BITS, &depthBits);
	glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
	
	print();
}
