#pragma once

#include "render.hpp"
#include <array>

namespace Render {
class Wall {
	Render& render;
	MaterialHandle mat;

	const int alloc_wall_count = 1000;
	int current_wall_count = 0;

	// struct Vertex {
	// 	vec3 pos, normal;
	// };

	struct MeshInstance {
		MeshHandle mesh;
		InstanceHandle instance;
	};
	std::vector<MeshInstance> wall_segements;

	std::optional<std::array<MeshDef::Vertex, 8>> last_frame;

  public:
	Wall(Render& render) : render(render) { mat = render.create_pbr_material({.albedoFactor = {0, 0.3, 0.7, 1.0}}); };
	Wall(Render& render, MaterialHandle material) : render(render), mat(material){};
	Wall(Wall& wall) = delete;

	void append_wall(mat4 bikeTransform, vec3 position, vec2 scale) {
		vec3 points[] = {
			{-scale.x, -scale.y, 0.0}, {-scale.x, scale.y, 0.0}, {scale.x, scale.y, 0.0}, {scale.x, -scale.y, 0.0}};

		for (auto& point : points) {
			point += position;
			vec4 temp = bikeTransform * vec4{point.x, point.y, point.z, 1};
			point = {temp.x, temp.y, temp.z};
		}

		vec3 normals[] = {{-1, 0, 0}, {0, 1, 0}, {1, 0, 0}, {0, -1, 0}};

		for (auto& normal : normals) {
			vec4 temp = bikeTransform * vec4{normal.x, normal.y, normal.z, 0};
			normal = {temp.x, temp.y, temp.z};
		}

		// clang-format off
		std::array<MeshDef::Vertex, 8> vertices = {
			MeshDef::Vertex{points[0], normals[0], {0.0, 0.0}},
			MeshDef::Vertex{points[1], normals[0], {0.0, 0.0}},
			MeshDef::Vertex{points[1], normals[1], {0.0, 0.0}},
			MeshDef::Vertex{points[2], normals[1], {0.0, 0.0}},
			MeshDef::Vertex{points[2], normals[2], {0.0, 0.0}},
			MeshDef::Vertex{points[3], normals[2], {0.0, 0.0}},
			MeshDef::Vertex{points[3], normals[3], {0.0, 0.0}},
			MeshDef::Vertex{points[0], normals[3], {0.0, 0.0}},
		};
		// clang-format on

		if (last_frame.has_value()) {
			// clang-format off
			auto mesh = render.create_mesh(MeshDef{
				.verticies = std::vector{
						(*last_frame)[0], (*last_frame)[1], (*last_frame)[2], (*last_frame)[3],
						(*last_frame)[4], (*last_frame)[5], (*last_frame)[6], (*last_frame)[7],
						vertices[0], vertices[1], vertices[2], vertices[3],
						vertices[4], vertices[5], vertices[6], vertices[7]},
				.indicies = {
					0 + 0, 8 + 0, 1 + 0, 1 + 0, 8 + 0, 9 + 0,
					0 + 2, 8 + 2, 1 + 2, 1 + 2, 8 + 2, 9 + 2,
					0 + 4, 8 + 4, 1 + 4, 1 + 4, 8 + 4, 9 + 4,
					0 + 6, 8 + 6, 1 + 6, 1 + 6, 8 + 6, 9 + 6,
					}});
			// clang-format on

			auto instance = render.create_instance(mesh, mat);
			wall_segements.push_back(MeshInstance{.mesh = mesh, .instance = instance});
		}
		last_frame.emplace(vertices);
	}
};
} // namespace Render