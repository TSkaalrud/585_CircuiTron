#include "wall.hpp"

#include <gl.hpp>

namespace Render {
void Wall::append_wall(mat4 bikeTransform, vec3 position, vec2 scale) {
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

	if (wall_segements.size() != 0) {
		auto& wall = wall_segements.back();
		auto bufferSlot = ((current_wall_count % alloc_wall_count) + 1);

		glNamedBufferSubData(wall.vertex_buffer, bufferSlot * sizeof(Vertex) * 8, sizeof(Vertex) * 8, vertices.data());
	}

	if (frame++ < frame_delay)
		return;
	frame = 0;
	current_wall_count++;

	if (current_wall_count % alloc_wall_count == 0) {
		GLuint vertex_buffer, index_buffer, vao;
		glCreateBuffers(1, &vertex_buffer);
		glCreateBuffers(1, &index_buffer);
		glCreateVertexArrays(1, &vao);

		auto num_buffer_slots = alloc_wall_count + 1;
		glNamedBufferStorage(vertex_buffer, num_buffer_slots * sizeof(Vertex) * 8, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(index_buffer, alloc_wall_count * sizeof(uint32_t) * 24, nullptr, GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(vao, 0, vertex_buffer, 0, sizeof(Vertex));
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, offsetof(Vertex, pos));
		glVertexArrayVertexBuffer(vao, 1, vertex_buffer, 0, sizeof(Vertex));
		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, offsetof(Vertex, normal));

		glNamedBufferSubData(vertex_buffer, 0, sizeof(Vertex) * 8, vertices.data());

		glVertexArrayElementBuffer(vao, index_buffer);

		auto mesh = render.registerMesh({.vao = vao, .count = 0});

		auto instance = render.create_instance(mesh, mat);
		wall_segements.push_back(MeshInstance{
			.mesh = mesh,
			.instance = instance,
			.vertex_buffer = vertex_buffer,
			.index_buffer = index_buffer,
			.vao = vao});
	}

	auto& wall = wall_segements.back();
	{
		auto bufferSlot = current_wall_count % alloc_wall_count;
		uint32_t offset = bufferSlot * 8;
		std::array<uint32_t, 24> indicies = {
			0 + 0 + offset, 8 + 0 + offset, 1 + 0 + offset, 1 + 0 + offset, 8 + 0 + offset, 9 + 0 + offset,
			0 + 2 + offset, 8 + 2 + offset, 1 + 2 + offset, 1 + 2 + offset, 8 + 2 + offset, 9 + 2 + offset,
			0 + 4 + offset, 8 + 4 + offset, 1 + 4 + offset, 1 + 4 + offset, 8 + 4 + offset, 9 + 4 + offset,
			0 + 6 + offset, 8 + 6 + offset, 1 + 6 + offset, 1 + 6 + offset, 8 + 6 + offset, 9 + 6 + offset,
		};
		glNamedBufferSubData(
			wall.index_buffer, (current_wall_count % alloc_wall_count) * sizeof(uint32_t) * 24, sizeof(uint32_t) * 24,
			indicies.data());
		render.meshes[wall.mesh].count += 24;
	}
	{
		auto bufferSlot = ((current_wall_count % alloc_wall_count) + 1);

		glNamedBufferSubData(wall.vertex_buffer, bufferSlot * sizeof(Vertex) * 8, sizeof(Vertex) * 8, vertices.data());
	}
}
} // namespace Render