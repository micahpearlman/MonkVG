#include "glShader.h"
#include <stdio.h>

namespace MonkVG {
OpenGLShader::OpenGLShader() {
    _program         = glCreateProgram();
    _vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
    _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
}

OpenGLShader::~OpenGLShader() {
    glDeleteShader(_vertex_shader);
    glDeleteShader(_fragment_shader);
    glDeleteProgram(_program);
}

bool OpenGLShader::compile(const char *vertexSource,
                           const char *fragmentSource) {
    GLint status;
    glShaderSource(_vertex_shader, 1, &vertexSource, 0);
    glCompileShader(_vertex_shader);
    glGetShaderiv(_vertex_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(_vertex_shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = new GLchar[length];
        glGetShaderInfoLog(_vertex_shader, length, &length, log);
        printf("OpenGLShader: vertex shader compile log:\n%s\n", log);
        delete[] log;
        return false;
    }

    glShaderSource(_fragment_shader, 1, &fragmentSource, 0);
    glCompileShader(_fragment_shader);
    glGetShaderiv(_fragment_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(_fragment_shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = new GLchar[length];
        glGetShaderInfoLog(_fragment_shader, length, &length, log);
        printf("OpenGLShader: fragment shader compile log:\n%s\n", log);
        delete[] log;
        return false;
    }

    glAttachShader(_program, _vertex_shader);
    glAttachShader(_program, _fragment_shader);
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

    // get uniform locations
    _u_projection = glGetUniformLocation(_program, "u_projection");
    _u_model_view = glGetUniformLocation(_program, "u_model_view");
    _u_color      = glGetUniformLocation(_program, "u_color");

    return true;
}

void OpenGLShader::bind() { glUseProgram(_program); }

void OpenGLShader::unbind() { glUseProgram(0); }

void OpenGLShader::setUniform1i(const char *name, int value) {
    GLint location = glGetUniformLocation(_program, name);
    glUniform1i(location, value);
}
void OpenGLShader::setUniform1f(const char *name, float value) {
    GLint location = glGetUniformLocation(_program, name);
    glUniform1f(location, value);
}
void OpenGLShader::setUniform2f(const char *name, float x, float y) {
    GLint location = glGetUniformLocation(_program, name);
    glUniform2f(location, x, y);
}
void OpenGLShader::setUniform3f(const char *name, float x, float y, float z) {
    GLint location = glGetUniformLocation(_program, name);
    glUniform3f(location, x, y, z);
}
void OpenGLShader::setUniform4f(const char *name, float x, float y, float z,
                                float w) {
    GLint location = glGetUniformLocation(_program, name);
    glUniform4f(location, x, y, z, w);
}
void OpenGLShader::setUniformMatrix4fv(const char *name, float *matrix) {
    GLint location = glGetUniformLocation(_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}
GLuint OpenGLShader::getUniformLocation(const char *name) {
    return glGetUniformLocation(_program, name);
}

void OpenGLShader::setProjectionMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(_u_projection, 1, GL_FALSE, &matrix[0][0]);
}
void OpenGLShader::setModelViewMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(_u_model_view, 1, GL_FALSE, &matrix[0][0]);
}

// color setter
// NOTE: this assumes there is a uniform in the shader called "u_color"
void OpenGLShader::setColor(const glm::vec4 &color) {
    glUniform4f(_u_color, color.r, color.g, color.b, color.a);
}

} // namespace MonkVG