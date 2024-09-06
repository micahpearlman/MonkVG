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
#include <glm/glm.hpp>

namespace MonkVG {
class OpenGLShader {
  public:
    OpenGLShader();
    virtual ~OpenGLShader();
    bool compile(const char *vertexSource, const char *fragmentSource);
    void bind();
    void unbind();

    // uniform setters
    void setUniform1i(const char *name, int value);
    void setUniform1f(const char *name, float value);
    void setUniform2f(const char *name, float x, float y);
    void setUniform3f(const char *name, float x, float y, float z);
    void setUniform4f(const char *name, float x, float y, float z, float w);
    void setUniformMatrix4fv(const char *name, float *matrix);

    // uniform getters
    GLuint getUniformLocation(const char *name);

    // projection and modelview setters
    // NOTE: this assumes there is a uniform in the shader called "u_projection"
    // and "u_model_view"
    void setProjectionMatrix(const glm::mat4 &matrix);
    void setModelViewMatrix(const glm::mat4 &matrix);

    // color setter
    // NOTE: this assumes there is a uniform in the shader called "u_color"
    void setColor(const glm::vec4 &color);

  private:
    GLuint _program         = -1;
    GLuint _vertex_shader   = -1;
    GLuint _fragment_shader = -1;

    GLint _u_projection = -1;
    GLint _u_model_view = -1;
    GLint _u_color      = -1;
};
} // namespace MonkVG
#endif // __glShader_h__
