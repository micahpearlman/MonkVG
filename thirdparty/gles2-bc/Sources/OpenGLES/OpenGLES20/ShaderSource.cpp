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

#include <cstring>
#include <cstdlib>
#include "ShaderSource.h"
#include "ShaderFile.h"
#include "OpenGLESUtil.h"
#include "OpenGLESConfig.h"
#include "shaders/all_shaders.h"

using namespace OpenGLES::OpenGLES2;

ShaderSource::ShaderSource( ShaderFile *file) : file(file), additionalSource(), sourceExpanded(false)
{
	
}

ShaderSource::ShaderSource( ShaderFile *file, std::string additionalSource ) : file(file), additionalSource(additionalSource), sourceExpanded(false)
{
	
}

bool ShaderSource::expandSource()
{
	static char tmp[1024*16];
	size_t n;
	// Check if file is already in memory...
	LOG_DEBUG_MESSAGE("Request for " + file->getName());
	int i;
	for (i = 0; i < SHADERMAP_CNT; ++i)
		if (file->getName() == ::shaders[i].name)
			break;
	if (i < SHADERMAP_CNT)
	{
		// Have in memory - just set length and copy over to tmp...
		n = ::shaders[i].len;
		memcpy(tmp, ::shaders[i].shader, n);
		LOG_DEBUG_MESSAGE(OpenGLESString("File in memory; length ") + n);
	}
	else
	{
		// Not in memory - fall back to file.
		LOG_DEBUG_MESSAGE("File not in memory?");
		if (!file->open())
		{
			LOG_DEBUG_MESSAGE(OpenGLESString("ERROR: Cannot open file ") + file->getName());
			return false;
		}
		file->seek(0, SEEK_END);
		long pos = file->tell();
		file->seek(0, SEEK_SET);

		n = file->read(tmp, 1, pos);
		file->close();
	}
	tmp[n] = '\0';

	int additionalSourceLength = (int)additionalSource.size();
	char *sourceTmp = (char *)malloc(sizeof(char) * n + additionalSourceLength + 1);
	if (sourceTmp == NULL)
	{
		LOG_MESSAGE(__FILE__, __LINE__, "ERROR: Cannot allocate memory.");
		return false;
	}

	memcpy(sourceTmp, additionalSource.c_str(), additionalSource.size());
	memcpy(sourceTmp + additionalSource.size(), tmp, n);
	sourceTmp[additionalSource.size() + n] = 0;
	LOG_DEBUG_MESSAGE(OpenGLESString("Final source is:") + strlen(sourceTmp) + " = " + (additionalSource.size() + n) + " bytes");
//	LOG_MESSAGE(OpenGLESString(additionalSource.c_str()) + "\n+\n" + tmp + "\n=\n" + sourceTmp);

	source = sourceTmp;
	sourceExpanded = true;
	free(sourceTmp);

	return true;
}

std::string ShaderSource::getSource()
{
	if (!sourceExpanded) {
		expandSource();
	}
	return source;
}

ShaderFile* ShaderSource::getFile()
{
	return file;
}

void ShaderSource::appendAdditionalSource( std::string newAdditionalSource )
{
	additionalSource += newAdditionalSource;
	sourceExpanded = false;
}
