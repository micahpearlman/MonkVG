#include "glShader.h"
#include <stdio.h>

namespace MonkVG {
OpenGLShader::OpenGLShader() {
    _program        = glCreateProgram();
    _vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
}

OpenGLShader::~OpenGLShader() {
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
    glDeleteProgram(_program);
}

bool OpenGLShader::compile(const char *vertexSource,
                           const char *fragmentSource) {
    GLint status;
    glShaderSource(_vertexShader, 1, &vertexSource, 0);
    glCompileShader(_vertexShader);
    glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(_vertexShader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = new GLchar[length];
        glGetShaderInfoLog(_vertexShader, length, &length, log);
        printf("OpenGLShader: vertex shader compile log:\n%s\n", log);
        delete[] log;
        return false;
    }

    glShaderSource(_fragmentShader, 1, &fragmentSource, 0);
    glCompileShader(_fragmentShader);
    glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(_fragmentShader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = new GLchar[length];
        glGetShaderInfoLog(_fragmentShader, length, &length, log);
        printf("OpenGLShader: fragment shader compile log:\n%s\n", log);
        delete[] log;
        return false;
    }

    glAttachShader(_program, _vertexShader);
    glAttachShader(_program, _fragmentShader);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = new GLchar[length];
        glGetProgramInfoLog(_program, length, &length, log);
        printf("OpenGLShader: program link log:\n%s\n", log);
        delete[] log;
        return false;
    }

    return true;
}

void OpenGLShader::bind() { glUseProgram(_program); }

void OpenGLShader::unbind() { glUseProgram(0); }
} // namespace MonkVG