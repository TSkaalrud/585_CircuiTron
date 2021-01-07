#pragma once
#include <GL/glew.h>
inline GLenum loadGL(void (*(const char*))()) { return glewInit(); }