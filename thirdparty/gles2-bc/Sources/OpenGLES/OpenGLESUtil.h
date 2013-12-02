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

#ifndef OpenGLESUtil_H_
#define OpenGLESUtil_H_

//#define OPENGLES_DEBUG  1

#ifdef OPENGLES_DEBUG
#define CHECK_GL_ERROR(X, Y, Z) OpenGLESUtil::checkGlError(X, Y, Z)
#define LOG_DEBUG_MESSAGE(...) OpenGLESUtil::logMessage((__VA_ARGS__))
#define PRINT(...) OpenGLESUtil::print(__VA_ARGS__)
#define PRINT_BITS(X) OpenGLESUtil::printBits(X)
#else
#define CHECK_GL_ERROR(X, Y, Z)
#define LOG_DEBUG_MESSAGE(...)
#define PRINT(...)
#define PRINT_BITS(X)
#endif

//#define LOG_MESSAGE(...) if (OpenGLESConfig::DEBUG) { OpenGLESUtil::logMessage((__VA_ARGS__)); }
#ifdef OPENGLES_DEBUG
#define LOG_MESSAGE(...) { OpenGLESUtil::logMessage((__VA_ARGS__)); }
#else
#define LOG_MESSAGE(...)
#endif

#include "OpenGLESContext.h"
#include "OpenGLESString.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace OpenGLES {
	
	class OpenGLESUtil {
	public:
		static void checkGlError(GLenum errorCode, const char *file, const unsigned int line);
		static void logMessage(const char *file, int line, OpenGLESString msg);
		static void logMessage(OpenGLESString msg);
		static void print( const char* format, ... );
		static void printBits(int val);
	};
	
}

#endif
