#include "core.hpp"

#include "debug.hpp"
#include "gl.hpp"

namespace Render {

struct Camera {
	mat4 camMat;
	vec3 camPos;
};

Core::Core(void (*glGetProcAddr(const char*))()) {
	loadGL(glGetProcAddr);

	loadDebugger();

	glEnable(GL_FRAMEBUFFER_SRGB);

	glCreateBuffers(1, &cameraBuffer);
	glNamedBufferStorage(cameraBuffer, sizeof(Camera), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer);

	glCreateBuffers(1, &dirLightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dirLightBuffer);

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &dirLightShadow);
	glTextureStorage3D(dirLightShadow, 1, GL_DEPTH_COMPONENT32F, lightmapSize, lightmapSize, 8);
	glTextureParameteri(dirLightShadow, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTextureParameteri(dirLightShadow, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
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

uint Core::create_texture(int width, int height, int channels, bool srgb, void* data) {
	GLuint texture;
	GLenum format, internalformat;
	switch (channels) {
	case 1:
		format = GL_RED;
		internalformat = GL_R8;
		break;
	case 2:
		format = GL_RG;
		internalformat = GL_RG8;
		break;
	case 3:
		format = GL_RGB;
		internalformat = srgb ? GL_SRGB8 : GL_RGB8;
		break;
	case 4:
		format = GL_RGBA;
		internalformat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
		break;
	}
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, glm::max(glm::log2(min(width, height)), 1), internalformat, width, height);
	glTextureParameterf(texture, GL_TEXTURE_MAX_ANISOTROPY, INFINITY);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	glGenerateTextureMipmap(texture);
	return texture;
}

void Core::renderScene() {
	for (auto& i : instances) {
		glUseProgram(shaders[materials[i.mat].shader].shader);
		glBindVertexArray(meshes[i.model].vao);

		auto material = materials[i.mat];
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, material.uniform);
		glBindTextures(4, material.textures.size(), material.textures.data());

		glUniformMatrix4fv(0, 1, false, value_ptr(i.trans));

		glDrawElements(GL_TRIANGLES, meshes[i.model].count, GL_UNSIGNED_INT, 0);
	}
}

void Core::run() {
	glClearColor(0, 0.2, 0.5, 1.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glNamedBufferData(dirLightBuffer, vector_size(dirLights), dirLights.data(), GL_DYNAMIC_DRAW);

	for (size_t i = 0; i < dirLights.size(); i++) {
		GLuint framebuffer;
		glCreateFramebuffers(1, &framebuffer);
		glNamedFramebufferTextureLayer(framebuffer, GL_DEPTH_ATTACHMENT, dirLightShadow, 0, i);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		Camera cam = {.camMat = dirLights[i].shadowMapTrans, .camPos = dirLights[i].dir};
		glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

		glViewport(0, 0, lightmapSize, lightmapSize);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		renderScene();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &framebuffer);
	}

	Camera cam = {
		.camMat = infinitePerspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f) * cameraPos,
		.camPos = vec3(inverse(cameraPos) * vec4{0, 0, 0, 1})};
	glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTextures(0, 1, &dirLightShadow);
	glCullFace(GL_BACK);
	renderScene();
}

} // namespace Render