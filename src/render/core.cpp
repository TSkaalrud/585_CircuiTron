#include "core.hpp"

#include <algorithm>

#include "debug.hpp"
#include "gl.hpp"

namespace Render {

Core::Core(void (*glGetProcAddr(const char*))()) {
	loadGL(glGetProcAddr);

	loadDebugger();

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

	return register_mesh(
		Mesh{.vao = vao, .count = static_cast<uint>(def.indicies.size()), .buffers = {vertex_buffer, index_buffer}});
}

void Core::delete_mesh(MeshHandle handle) {
	glDeleteVertexArrays(1, &(meshes[handle].vao));

	glDeleteBuffers(meshes[handle].buffers.size(), meshes[handle].buffers.data());

	meshes[handle].count = -1;
	recycled_meshes.push_back(handle);
}

uint Core::create_texture(int width, int height, int channels, TextureFlags flags, void* data) {
	bool srgb = flags & TextureFlags::SRGB;
	bool mipmaps = flags & TextureFlags::MIPMAPPED;
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
	auto levels = 1;
	if (mipmaps)
		levels = glm::max(glm::log2(min(width, height)), 1);
	glTextureStorage2D(texture, levels, internalformat, width, height);
	if (flags & TextureFlags::ANIOSTROPIC)
		glTextureParameterf(texture, GL_TEXTURE_MAX_ANISOTROPY, INFINITY);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	if (flags & TextureFlags::CLAMPED) {
		glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if (mipmaps)
		glGenerateTextureMipmap(texture);
	return texture;
}

void Core::renderScene(Shader::Type type, RenderOrder order) {
	if (order == RenderOrder::Shader) {
		for (int i = 0; i < shaders.size(); i++) {
			auto& shader = shaders[i];
			if ((shader.type & type) == 0)
				continue;

			glUseProgram(shader.shader);
			for (auto mat : shader.materials) {
				auto& material = materials[mat];

				for (auto shaderConfig : material.shaders) {
					if (shaderConfig.shader != i)
						continue;
					glBindBufferBase(GL_UNIFORM_BUFFER, 1, shaderConfig.uniform);
					glBindTextures(3, shaderConfig.textures.size(), shaderConfig.textures.data());
					break;
				}

				for (auto i : material.instances) {
					auto& instance = instances[i];
					if (meshes[instance.model].count == -1)
						continue;
					glBindVertexArray(meshes[instance.model].vao);

					glUniformMatrix4fv(0, 1, false, value_ptr(instance.trans));

					glDrawElements(GL_TRIANGLES, meshes[instance.model].count, GL_UNSIGNED_INT, 0);
				}
			}
		}
	} else {
		auto sorted_instances = instances;
		if (order == RenderOrder::UIDepth) {
			std::sort(sorted_instances.begin(), sorted_instances.end(), [](Instance a, Instance b) {
				return a.trans[3][2] < b.trans[3][2];
			});
		}
		for (auto& i : sorted_instances) {
			if (i.mat == -1)
				continue;
			if (meshes[i.model].vao == -1)
				continue;

			glBindVertexArray(meshes[i.model].vao);
			for (auto& shader : materials[i.mat].shaders) {
				if ((shaders[shader.shader].type & type) == 0)
					continue;

				glUseProgram(shaders[shader.shader].shader);

				glBindBufferBase(GL_UNIFORM_BUFFER, 1, shader.uniform);
				glBindTextures(3, shader.textures.size(), shader.textures.data());

				glUniformMatrix4fv(0, 1, false, value_ptr(i.trans));

				glDrawElements(GL_TRIANGLES, meshes[i.model].count, GL_UNSIGNED_INT, 0);
			}
		}
	}
}

void Core::run() {
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);

	glNamedBufferData(dirLightBuffer, vector_size(dirLights), dirLights.data(), GL_DYNAMIC_DRAW);

	GLuint framebuffer;
	glCreateFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	for (size_t i = 0; i < dirLights.size(); i++) {
		glNamedFramebufferTextureLayer(framebuffer, GL_DEPTH_ATTACHMENT, dirLightShadow, 0, i);

		Camera cam = {.proj = mat4(1.0f), .view = dirLights[i].shadowMapTrans, .camPos = dirLights[i].dir};
		glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

		glViewport(0, 0, lightmapSize, lightmapSize);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		renderScene(Shader::Type::Shadow);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &framebuffer);

	Camera cam = {
		.proj = infinitePerspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f),
		.view = cameraPos,
		.camPos = vec3(inverse(cameraPos) * vec4{0, 0, 0, 1})};
	glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTextures(0, 1, &dirLightShadow);
	glCullFace(GL_BACK);
	renderScene(Shader::Type::Opaque | Shader::Type::Skybox);

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderScene(Shader::Type::UI, RenderOrder::UIDepth);
}

} // namespace Render