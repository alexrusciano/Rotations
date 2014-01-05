#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

void LogMessage ( const char *formatStr, ... );

GLuint LoadShader ( GLenum type, const char *shaderSrc );

GLuint LoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );

#endif