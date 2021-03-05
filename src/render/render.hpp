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
	const int skyboxSize = 8192;

	InstanceHandle skybox;
	TextureHandle skyboxCubemap;

	void render_cubemap(Shader::Type type, GLuint cubemap, int width);

  public:
	Render(void (*glGetProcAddr(const char*))());
	Render(const Render&) = delete;

	MaterialHandle create_pbr_material(MaterialPBR);

	void set_skybox_material(MaterialHandle material, bool update = true) {
		instance_set_material(skybox, material);
		if (update)
			update_skybox();
	}
	void set_skybox_rect_texture(TextureHandle, bool update = true);
	void set_skybox_cube_texture(TextureHandle, bool update = true);

	void update_skybox();
};

void render_test();
} // namespace Render