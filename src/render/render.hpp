#pragma once

#include "core.hpp"

namespace Render {

struct MaterialPBR {
	vec4 albedoFactor;
	float metalFactor;
	float roughFactor;
	// No textures above here
	// Only textures below here
	// albedo must be first
	TextureHandle albedoTexture;
	TextureHandle metalRoughTexture;
};

class Render : public Core {
  public:
	Render(void (*glGetProcAddr(const char*))()) : Core(glGetProcAddr) {}
	Render(const Render&) = delete;

	MaterialHandle create_pbr_material(MaterialPBR);
};

void render_test();
} // namespace Render