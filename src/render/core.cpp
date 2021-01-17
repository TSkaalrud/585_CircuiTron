#include "core.hpp"

#include "debug.hpp"
#include "gl.hpp"
#include <glm/gtc/integer.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Render {

struct Camera {
	mat4 view;
	mat4 proj;
	vec3 cameraPos;
};

Core::Core(void (*glGetProcAddr(const char*))()) {
	loadGL(glGetProcAddr);

	loadDebugger();

	glCreateBuffers(1, &cameraBuffer);
	glNamedBufferStorage(cameraBuffer, sizeof(Camera), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer);

	glCreateBuffers(1, &dirLightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dirLightBuffer);
}

template <class T> size_t vector_size(const std::vector<T>& vec) { return sizeof(T) * vec.size(); }

uint Core::create_mesh(MeshDef def) {

	GLuint vertex_buffer, index_buffer, vao;
	glCreateBuffers(1, &vertex_buffer);
	glCreateBuffers(1, &index_buffer);
	glCreateVertexArrays(1, &vao);

	glNamedBufferStorage(vertex_buffer, vector_size(def.verticies), def.verticies.data(), 0);
	glNamedBufferStorage(index_buffer, vector_size(def.indicies), def.indicies.data(), 0);

	glVertexArrayVertexBuffer(vao, 0, vertex_buffer, 0, sizeof(MeshDef::Vertex));
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, offsetof(MeshDef::Vertex, pos));
	glVertexArrayVertexBuffer(vao, 1, vertex_buffer, 0, sizeof(MeshDef::Vertex));
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, offsetof(MeshDef::Vertex, normal));
	glVertexArrayVertexBuffer(vao, 2, vertex_buffer, 0, sizeof(MeshDef::Vertex));
	glEnableVertexArrayAttrib(vao, 2);
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, false, offsetof(MeshDef::Vertex, uv));

	glVertexArrayElementBuffer(vao, index_buffer);

	uint handle = meshes.size();
	meshes.push_back(Mesh{.vao = vao, .count = static_cast<uint>(def.indicies.size())});
	return handle;
}

uint Core::create_texture(int width, int height, void* data) {
	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, log2(min(width, height)), GL_RGBA8, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateTextureMipmap(texture);
	return texture;
}

void Core::run() {
	glViewport(0, 0, width, height);
	glClearColor(0, 0.5, 0.8, 1.0);

	Camera cam = {
		.view = cameraPos,
		.proj = infinitePerspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f),
		.cameraPos = vec3(inverse(cameraPos) * vec4{0, 0, 0, 1})};
	glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

	glNamedBufferData(dirLightBuffer, vector_size(dirLights), dirLights.data(), GL_DYNAMIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& i : instances) {
		glUseProgram(shaders[materials[i.mat].shader].shader);
		glBindVertexArray(meshes[i.model].vao);

		auto material = materials[i.mat];
		// glUniform4fv(1, 1, value_ptr(material.pbr.albedo));
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, material.uniform);
		glBindTextures(4, material.textures.size(), material.textures.data());

		glUniformMatrix4fv(0, 1, false, value_ptr(i.trans));

		glDrawElements(GL_TRIANGLES, meshes[i.model].count, GL_UNSIGNED_INT, 0);
	}
}

} // namespace Render