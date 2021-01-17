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

MaterialHandle Render::create_pbr_material(MaterialPBR pbr) {
	GLuint shader =
		load_shader_program({{"shaders/default.vert", GL_VERTEX_SHADER}, {"shaders/pbr.frag", GL_FRAGMENT_SHADER}});
	uint shaderHandle = shaders.size();
	shaders.push_back({shader});

	GLuint uniform;
	glCreateBuffers(1, &uniform);
	glNamedBufferStorage(uniform, offsetof(MaterialPBR, albedoTexture), &pbr, 0);

	// Scary memcpy stuff
	// It's probably fine
	// I will regret doing this
	const int textureSize = sizeof(MaterialPBR) - offsetof(MaterialPBR, albedoTexture);
	std::vector<TextureHandle> textures;
	textures.resize(textureSize / sizeof(TextureHandle));
	memcpy(textures.data(), &pbr.albedoTexture, textureSize);

	uint handle = materials.size();
	materials.push_back(Material{.shader = shaderHandle, .uniform = uniform, .textures = textures});
	return handle;
}
} // namespace Render