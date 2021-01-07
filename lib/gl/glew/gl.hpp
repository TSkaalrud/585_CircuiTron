#pragma once
#define GLEW_NO_GLU
#include <GL/glew.h>
inline GLenum loadGL(void (*(const char*))()) { return glewInit(); }
