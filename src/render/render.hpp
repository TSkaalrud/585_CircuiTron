#pragma once

#include "core.hpp"

namespace Render {

class Render : public Core {
  public:
	Render(void (*glGetProcAddr(const char*))()) : Core(glGetProcAddr) {}
	Render(const Render&) = delete;
};

void render_test();
} // namespace Render