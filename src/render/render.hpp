#pragma once

#include "core.hpp"

namespace Render {

struct MaterialPBR {
	vec4 albedoFactor;
	TextureHandle albedoTexture;
	float metalFactor;
	float roughFactor;
	TextureHandle metalRoughTexture;
	vec3 emissiveFactor;
	TextureHandle emissiveTexture;
};

class Render : public Core {
  public:
	Render(void (*glGetProcAddr(const char*))()) : Core(glGetProcAddr) {}
	Render(const Render&) = delete;

	MaterialHandle create_pbr_material(MaterialPBR);
};

void render_test();
} // namespace Render