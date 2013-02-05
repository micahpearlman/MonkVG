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

#include "OpenGLESFile.h"
#include <stdio.h>

using namespace OpenGLES;

std::string OpenGLESFile::rootPath( "/" );

OpenGLESFile::OpenGLESFile(std::string n) : name()
{
	name = n;
}

bool OpenGLESFile::open() 
{
	std::string p = getPath();
        fp = fopen(p.c_str(), "r");
	
	return fp;
}

char * OpenGLESFile::gets(char * buf, int size)
{
	return fgets(buf, size, fp);
}

int OpenGLESFile::seek(long int offset, int origin)
{
	return fseek(fp, offset, origin);
}

long int OpenGLESFile::tell()
{
	return ftell(fp);
}

size_t OpenGLESFile::read(void *ptr, size_t size, size_t count)
{
	return fread(ptr, size, count, fp);
}

void OpenGLESFile::close()
{
	fclose(fp);
}

std::string OpenGLESFile::getPath() const
{
	return OpenGLESFile::rootPath + name;
}

std::string OpenGLESFile::getName() const
{
	return name;
}
