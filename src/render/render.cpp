#include "render.hpp"

#include "gl.hpp"
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

namespace Render {

struct ShaderStage {
	std::string filename;
	GLenum shaderType;
};

GLuint load_shader_program(std::vector<ShaderStage> stages) {
	auto program = glCreateProgram();

	for (auto stage : stages) {
		std::string shaderCode;
		std::ifstream shaderFile;
		shaderFile.open(stage.filename);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
		const char* cShaderCode = shaderCode.c_str();
		auto shader = glCreateShader(stage.shaderType);
		glShaderSource(shader, 1, &cShaderCode, 0);
		glCompileShader(shader);
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	return program;
}

struct PBR {
	vec4 albedoFactor;
	vec3 emissiveFactor;
	float metalFactor;
	float roughFactor;
	float reflectionLevels;
};
MaterialHandle Render::create_pbr_material(MaterialPBR pbr) {
	static ShaderHandle shader = registerShader(Shader{
		load_shader_program({{"shaders/default.vert", GL_VERTEX_SHADER}, {"shaders/pbr.frag", GL_FRAGMENT_SHADER}}),
		Shader::Type::Opaque});
	static ShaderHandle depthShader = registerShader(Shader{
		load_shader_program({{"shaders/default.vert", GL_VERTEX_SHADER}, {"shaders/depth.frag", GL_FRAGMENT_SHADER}}),
		Shader::Type::Depth | Shader::Type::Shadow});

	GLuint uniform;
	glCreateBuffers(1, &uniform);
	PBR _pbr = {
		.albedoFactor = pbr.albedoFactor,
		.emissiveFactor = pbr.emissiveFactor,
		.metalFactor = pbr.metalFactor,
		.roughFactor = pbr.roughFactor,
		.reflectionLevels = static_cast<float>(reflectionLevels),
	};
	glNamedBufferStorage(uniform, sizeof(PBR), &_pbr, 0);

	static uint8_t white[3] = {255, 255, 255};
	static auto whiteTexture = create_texture(1, 1, 3, false, &white);

	std::vector<TextureHandle> textures = {
		irradiance,
		reflection,
		reflectionBRDF,
		pbr.albedoTexture.value_or(whiteTexture),
		pbr.metalRoughTexture.value_or(whiteTexture),
		pbr.emissiveTexture.value_or(whiteTexture)};

	return register_material(Material{
		{{.shader = shader, .uniform = uniform, .textures = textures},
		 {.shader = depthShader, .uniform = 0, .textures = {}}}});
}

Render::Render(void (*glGetProcAddr(const char*))()) : Core(glGetProcAddr) {
	{
		ShaderHandle skyboxShader = registerShader(Shader{
			load_shader_program(
				{{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/testSkybox.frag", GL_FRAGMENT_SHADER}}),
			Shader::Type::Skybox});
		MaterialHandle skyboxMaterial =
			register_material(Material{{{.shader = skyboxShader, .uniform = 0, .textures = {}}}});

		std::vector<vec3> verticies = {
			{-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1}, {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
		};

		// clang-format off
	std::vector<uint32_t> indicies = {
		2, 0, 4, 4, 6, 2,
		1, 0, 2, 2, 3, 1,
		4, 5, 7, 7, 6, 4,
		1, 3, 7, 7, 5, 1,
		2, 6, 7, 7, 3, 2,
		0, 1, 4, 4, 1, 5,
	};
		// clang-format on

		GLuint vertex_buffer, index_buffer, vao;
		glCreateBuffers(1, &vertex_buffer);
		glCreateBuffers(1, &index_buffer);
		glCreateVertexArrays(1, &vao);

		glNamedBufferStorage(vertex_buffer, vector_size(verticies), verticies.data(), 0);
		glNamedBufferStorage(index_buffer, vector_size(indicies), indicies.data(), 0);

		glVertexArrayVertexBuffer(vao, 0, vertex_buffer, 0, sizeof(vec3));
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);

		glVertexArrayElementBuffer(vao, index_buffer);

		uint skyboxMesh = registerMesh(Mesh{.vao = vao, .count = static_cast<uint>(indicies.size())});
		skybox = create_instance(skyboxMesh, skyboxMaterial);
	}

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &skyboxCubemap);
	glTextureStorage2D(skyboxCubemap, 1, GL_RGB16F, skyboxSize, skyboxSize);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &irradiance);
	glTextureStorage2D(irradiance, 1, GL_RGB16F, irradianceSize, irradianceSize);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &reflection);
	glTextureStorage2D(reflection, reflectionLevels, GL_RGB16F, reflectionSize, reflectionSize);

	glCreateTextures(GL_TEXTURE_2D, 1, &reflectionBRDF);
	glTextureStorage2D(reflectionBRDF, 1, GL_RG16F, reflectionBRDFSize, reflectionBRDFSize);
	glTextureParameteri(reflectionBRDF, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(reflectionBRDF, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	{
		GLuint framebuffer;
		glCreateFramebuffers(1, &framebuffer);
		glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, reflectionBRDF, 0);
		glViewport(0, 0, reflectionBRDFSize, reflectionBRDFSize);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		static GLuint reflectionBRDFShader = load_shader_program(
			{{"shaders/quad.vert", GL_VERTEX_SHADER}, {"shaders/reflectionBRDF.frag", GL_FRAGMENT_SHADER}});
		glUseProgram(reflectionBRDFShader);
		mat3 transform(1.0f);
		glUniformMatrix3fv(0, 1, false, value_ptr(transform));

		GLuint vertex_buffer, index_buffer, quadVertexArray;
		glCreateBuffers(1, &vertex_buffer);
		glCreateBuffers(1, &index_buffer);
		glCreateVertexArrays(1, &quadVertexArray);

		vec2 quadverts[] = {
			{-1, -1}, {0, 0}, {-1, 1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}, {1, 1},
		};
		int quadindicies[] = {0, 1, 2, 2, 1, 3};

		glNamedBufferStorage(vertex_buffer, sizeof(quadverts), &quadverts, 0);
		glNamedBufferStorage(index_buffer, sizeof(quadindicies), &quadindicies, 0);

		glVertexArrayVertexBuffer(quadVertexArray, 0, vertex_buffer, 0, sizeof(vec2) * 2);
		glEnableVertexArrayAttrib(quadVertexArray, 0);
		glVertexArrayAttribFormat(quadVertexArray, 0, 2, GL_FLOAT, false, 0);
		glVertexArrayVertexBuffer(quadVertexArray, 1, vertex_buffer, 0, sizeof(vec2) * 2);
		glEnableVertexArrayAttrib(quadVertexArray, 1);
		glVertexArrayAttribFormat(quadVertexArray, 1, 2, GL_FLOAT, false, sizeof(vec2));

		glVertexArrayElementBuffer(quadVertexArray, index_buffer);

		glBindVertexArray(quadVertexArray);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDeleteVertexArrays(1, &quadVertexArray);
		glDeleteBuffers(1, &vertex_buffer);
		glDeleteBuffers(1, &index_buffer);

		glDeleteProgram(reflectionBRDFShader);
	}
}

const glm::mat4 captureViews[] = {
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

void Render::render_cubemap(Shader::Type type, RenderOrder order, GLuint cubemap, GLsizei size) {
	GLuint framebuffer, renderbuffer;
	glCreateFramebuffers(1, &framebuffer);
	glCreateRenderbuffers(1, &renderbuffer);
	glNamedRenderbufferStorage(renderbuffer, GL_DEPTH_COMPONENT24, size, size);
	glNamedFramebufferRenderbuffer(framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	for (int i = 0; i < 6; i++) {
		glNamedFramebufferTextureLayer(framebuffer, GL_COLOR_ATTACHMENT0, cubemap, 0, i);

		Camera cam = {
			.proj = infinitePerspective(glm::radians(90.0f), 1.0f, 0.1f),
			.view = captureViews[i],
			.camPos = vec3(0.0f)};
		glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

		glViewport(0, 0, size, size);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderScene(type, order);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteRenderbuffers(1, &renderbuffer);
}

void Render::update_skybox() {
	render_cubemap(Shader::Type::Skybox, RenderOrder::Shader, skyboxCubemap, skyboxSize);

	GLuint framebuffer;
	glCreateFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindVertexArray(meshes[skybox].vao);

	static GLuint irradianceShader = load_shader_program(
		{{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/irradiance.frag", GL_FRAGMENT_SHADER}});
	glUseProgram(irradianceShader);
	glBindTextures(1, 1, &skyboxCubemap);

	for (int i = 0; i < 6; i++) {
		glNamedFramebufferTextureLayer(framebuffer, GL_COLOR_ATTACHMENT0, irradiance, 0, i);

		Camera cam = {
			.proj = infinitePerspective(glm::radians(90.0f), 1.0f, 0.1f),
			.view = captureViews[i],
			.camPos = vec3(0.0f)};
		glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

		glViewport(0, 0, irradianceSize, irradianceSize);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, meshes[skybox].count, GL_UNSIGNED_INT, 0);
	}

	static GLuint reflectionShader = load_shader_program(
		{{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/reflection.frag", GL_FRAGMENT_SHADER}});
	glUseProgram(reflectionShader);
	glBindTextures(1, 1, &skyboxCubemap);

	for (int level = 0; level < reflectionLevels; level++) {
		glUniform1f(1, static_cast<float>(level) / static_cast<float>(reflectionLevels - 1));

		int levelSize = reflectionSize * pow(0.5, level);
		glViewport(0, 0, levelSize, levelSize);
		for (int i = 0; i < 6; i++) {
			glNamedFramebufferTextureLayer(framebuffer, GL_COLOR_ATTACHMENT0, reflection, level, i);

			Camera cam = {
				.proj = infinitePerspective(glm::radians(90.0f), 1.0f, 0.1f),
				.view = captureViews[i],
				.camPos = vec3(0.0f)};
			glNamedBufferSubData(cameraBuffer, 0, sizeof(Camera), &cam);

			glClear(GL_COLOR_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, meshes[skybox].count, GL_UNSIGNED_INT, 0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &framebuffer);

	// set_skybox_rect_texture(reflectionBRDF, false);
}

void Render::set_skybox_rect_texture(TextureHandle texture, bool update) {
	static ShaderHandle skyboxShader = registerShader(Shader{
		load_shader_program(
			{{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/rectSkybox.frag", GL_FRAGMENT_SHADER}}),
		Shader::Type::Skybox});
	static ShaderHandle skyboxDepthShader = registerShader(Shader{
		load_shader_program({{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/depth.frag", GL_FRAGMENT_SHADER}}),
		Shader::Type::Depth});
	static MaterialHandle skyboxMaterial = register_material(Material{
		{{.shader = skyboxShader, .uniform = 0, .textures = {texture}},
		 {.shader = skyboxDepthShader, .uniform = 0, .textures = {}}}});
	materials[skyboxMaterial].shaders[0].textures[0] = texture;
	set_skybox_material(skyboxMaterial, update);
}

void Render::set_skybox_cube_texture(TextureHandle texture, bool update) {
	static ShaderHandle skyboxShader = registerShader(Shader{
		load_shader_program(
			{{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/cubeSkybox.frag", GL_FRAGMENT_SHADER}}),
		Shader::Type::Skybox});
	static ShaderHandle skyboxDepthShader = registerShader(Shader{
		load_shader_program({{"shaders/skybox.vert", GL_VERTEX_SHADER}, {"shaders/depth.frag", GL_FRAGMENT_SHADER}}),
		Shader::Type::Depth});
	static MaterialHandle skyboxMaterial = register_material(Material{
		{{.shader = skyboxShader, .uniform = 0, .textures = {texture}},
		 {.shader = skyboxDepthShader, .uniform = 0, .textures = {}}}});
	materials[skyboxMaterial].shaders[0].textures[0] = texture;
	set_skybox_material(skyboxMaterial, update);
}

} // namespace Render