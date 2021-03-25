#pragma once

#include "render.hpp"
#include <array>

namespace Render {
class Wall {
	Render& render;
	MaterialHandle mat;

	const int alloc_wall_count = 1000;
	int current_wall_count = 0;

	const int frame_delay = 10;
	int frame = 0;

	struct Vertex {
		vec3 pos, normal;
	};

	struct MeshInstance {
		MeshHandle mesh;
		InstanceHandle instance;
	};
	std::vector<MeshInstance> wall_segements;

	std::optional<std::array<Vertex, 8>> last_frame;

  public:
	Wall(Render& render) : render(render) { mat = render.create_pbr_material({.albedoFactor = {0, 0.3, 0.7, 1.0}}); };
	Wall(Render& render, MaterialHandle material) : render(render), mat(material){};
	Wall(Wall& wall) = delete;

	void append_wall(mat4 bikeTransform, vec3 position, vec2 scale);
};
} // namespace Render