/**
 * @file glShader.h
 * @author Micah Pearlman (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-03
 *
 * @copyright Copyright Zero Engineering (c) 2024
 *
 */
#ifndef __glShader_h__
#define __glShader_h__

#include "glPlatform.h"

namespace MonkVG {
class OpenGLShader {
  public:
    OpenGLShader();
    ~OpenGLShader();
    bool compile(const char *vertexSource, const char *fragmentSource);
    void bind();
    void unbind();
    void setUniform1i(const char *name, int value);
    void setUniform1f(const char *name, float value);
    void setUniform2f(const char *name, float x, float y);
    void setUniform3f(const char *name, float x, float y, float z);
    void setUniform4f(const char *name, float x, float y, float z, float w);
    void setUniformMatrix4fv(const char *name, float *matrix);

  private:
    GLuint _program;
    GLuint _vertexShader;
    GLuint _fragmentShader;
};
} // namespace MonkVG
#endif // __glShader_h__
