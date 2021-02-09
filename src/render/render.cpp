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
struct _PBR {
	vec4 albedoFactor;
	vec3 emissiveFactor;
	float metalFactor;
	float roughFactor;
};

uint shaderHandle = -1;

MaterialHandle Render::create_pbr_material(MaterialPBR pbr) {
	if (shaderHandle == -1) {
		GLuint shader =
			load_shader_program({{"shaders/default.vert", GL_VERTEX_SHADER}, {"shaders/pbr.frag", GL_FRAGMENT_SHADER}});
		shaderHandle = shaders.size();
		shaders.push_back({shader});
	}

	GLuint uniform;
	glCreateBuffers(1, &uniform);
	_PBR _pbr = {
		.albedoFactor = pbr.albedoFactor,
		.emissiveFactor = pbr.emissiveFactor,
		.metalFactor = pbr.metalFactor,
		.roughFactor = pbr.roughFactor,
	};
	glNamedBufferStorage(uniform, sizeof(_PBR), &_pbr, 0);

	static uint8_t white[3] = {255, 255, 255};
	static auto whiteTexture = create_texture(1, 1, 3, false, &white);

	std::vector<TextureHandle> textures = {
		pbr.albedoTexture.value_or(whiteTexture), pbr.metalRoughTexture.value_or(whiteTexture),
		pbr.emissiveTexture.value_or(whiteTexture)};

	uint handle = materials.size();
	materials.push_back(Material{.shader = shaderHandle, .uniform = uniform, .textures = textures});
	return handle;
}
} // namespace Render