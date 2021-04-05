#pragma once

#include "render.hpp"
#include <array>

namespace Render {
class Wall {
	Render& render;
	MaterialHandle mat;

	const uint alloc_wall_count = 1000;
	uint current_wall_count = -1;

	struct Vertex {
		vec3 pos, normal;
	};

	struct MeshInstance {
		MeshHandle mesh;
		InstanceHandle instance;
		GLuint vertex_buffer, index_buffer, vao;
	};
	std::vector<MeshInstance> wall_segements;

  public:
	Wall(Render& render) : render(render) { mat = render.create_pbr_material({.albedoFactor = {0, 0.3, 0.7, 1.0}}); };
	Wall(Render& render, MaterialHandle material) : render(render), mat(material){};
	Wall(Wall& wall) = delete;

	void update_wall(mat4 bikeTransform, vec2 scale) { append_wall(bikeTransform, scale, false); }
	uint commit_wall(mat4 bikeTransform, vec2 scale) {
		append_wall(bikeTransform, scale, true);
		return current_wall_count - 1;
	}

	void delete_wall(uint wall);

  private:
	void append_wall(mat4 bikeTransform, vec2 scale, bool commit);
};
} // namespace Render