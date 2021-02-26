#pragma once

#include "core.hpp"
#include <optional>

namespace Render {

struct MaterialPBR {
	vec4 albedoFactor;
	std::optional<TextureHandle> albedoTexture;
	float metalFactor;
	float roughFactor;
	std::optional<TextureHandle> metalRoughTexture;
	vec3 emissiveFactor;
	std::optional<TextureHandle> emissiveTexture;
};

class Render : public Core {
	InstanceHandle skybox;

  public:
	Render(void (*glGetProcAddr(const char*))());
	Render(const Render&) = delete;

	MaterialHandle create_pbr_material(MaterialPBR);

	void set_skybox_material(MaterialHandle material) { instance_set_material(skybox, material); }
	void set_skybox_texture(TextureHandle);
};

void render_test();
} // namespace Render