#include "wall.hpp"

#include <gl.hpp>

namespace Render {
void Wall::append_wall(mat4 bikeTransform, vec3 position, vec2 scale) {
	if (frame++ < frame_delay)
		return;
	frame = 0;

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
		std::array<Vertex, 8> vertices = {
			Vertex{points[0], normals[0]},
			Vertex{points[1], normals[0]},
			Vertex{points[1], normals[1]},
			Vertex{points[2], normals[1]},
			Vertex{points[2], normals[2]},
			Vertex{points[3], normals[2]},
			Vertex{points[3], normals[3]},
			Vertex{points[0], normals[3]},
		};
	// clang-format on

	if (last_frame.has_value()) {
		// clang-format off
			std::vector<Vertex> verticies = {
				(*last_frame)[0], (*last_frame)[1], (*last_frame)[2], (*last_frame)[3],
				(*last_frame)[4], (*last_frame)[5], (*last_frame)[6], (*last_frame)[7],
				vertices[0], vertices[1], vertices[2], vertices[3],
				vertices[4], vertices[5], vertices[6], vertices[7]
			};
			std::vector<uint32_t> indicies = {
				0 + 0, 8 + 0, 1 + 0, 1 + 0, 8 + 0, 9 + 0,
				0 + 2, 8 + 2, 1 + 2, 1 + 2, 8 + 2, 9 + 2,
				0 + 4, 8 + 4, 1 + 4, 1 + 4, 8 + 4, 9 + 4,
				0 + 6, 8 + 6, 1 + 6, 1 + 6, 8 + 6, 9 + 6,
			};
		// clang-format on

		GLuint vertex_buffer, index_buffer, vao;
		glCreateBuffers(1, &vertex_buffer);
		glCreateBuffers(1, &index_buffer);
		glCreateVertexArrays(1, &vao);

		glNamedBufferStorage(vertex_buffer, Core::vector_size(verticies), verticies.data(), 0);
		glNamedBufferStorage(index_buffer, Core::vector_size(indicies), indicies.data(), 0);

		glVertexArrayVertexBuffer(vao, 0, vertex_buffer, 0, sizeof(Vertex));
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, offsetof(Vertex, pos));
		glVertexArrayVertexBuffer(vao, 1, vertex_buffer, 0, sizeof(Vertex));
		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, offsetof(Vertex, normal));

		glVertexArrayElementBuffer(vao, index_buffer);

		auto mesh = render.registerMesh({.vao = vao, .count = static_cast<uint>(indicies.size())});

		auto instance = render.create_instance(mesh, mat);
		wall_segements.push_back(MeshInstance{.mesh = mesh, .instance = instance});
	}
	last_frame.emplace(vertices);
}
} // namespace Render