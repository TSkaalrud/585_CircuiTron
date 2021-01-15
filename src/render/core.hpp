#pragma once

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

namespace Render {

using namespace glm;

typedef unsigned int GLuint;

struct MeshDef {
	struct Vertex {
		vec3 pos;
		vec3 normal;
	};
	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;
};
struct MaterialPBR {
	vec4 albedo;
	TextureHandle albedoTexture;
};

typedef uint MeshHandle;
typedef uint InstanceHandle;
typedef uint MaterialHandle;
typedef uint DirLightHandle;
typedef uint TextureHandle;

class Core {
  private:
	int width, height;

	GLuint cameraBuffer;
	float fov;
	mat4 cameraPos;

	struct Instance {
		uint model;
		uint mat;
		mat4 trans;
	};
	std::vector<Instance> instances;

	struct Mesh {
		GLuint vao;
		uint count;
	};
	std::vector<Mesh> meshes;

	struct Material {
		uint shader;
		MaterialPBR pbr;
	};
	std::vector<Material> materials;

	struct Shader {
		GLuint shader;
	};
	std::vector<Shader> shaders;

	struct DirLight {
		vec3 dir;
		float _pad0;
		vec3 colour;
		float _pad1;
	};
	std::vector<DirLight> dirLights;
	GLuint dirLightBuffer;

  public:
	Core(void (*(const char*))());
	Core(const Core&) = delete;

	void resize(int width, int height) {
		this->width = width;
		this->height = height;
	}

	MeshHandle create_mesh(MeshDef);
	uint create_pbr_material(MaterialPBR);
	InstanceHandle create_instance() {
		uint handle = instances.size();
		instances.push_back(Instance{});
		return handle;
	}
	void instance_set_mesh(InstanceHandle instance, MeshHandle mesh) { instances[instance].model = mesh; }
	void instance_set_material(InstanceHandle instance, MaterialHandle mat) { instances[instance].mat = mat; }
	void instance_set_trans(InstanceHandle instance, mat4 trans) { instances[instance].trans = trans; }

	void camera_set_pos(mat4 pos) { cameraPos = pos; }
	void camera_set_fov(float degrees) { fov = radians(degrees); }

	DirLightHandle create_dir_light(vec3 colour, vec3 dir) {
		uint handle = dirLights.size();
		dirLights.push_back(DirLight{});
		dir_light_set_colour(handle, colour);
		dir_light_set_dir(handle, dir);
		return handle;
	}
	void dir_light_set_colour(DirLightHandle handle, vec3 colour) { dirLights[handle].colour = colour; }
	void dir_light_set_dir(DirLightHandle handle, vec3 dir) { dirLights[handle].dir = normalize(dir); }

	TextureHandle create_texture(int width, int height, void* data);

	void run();
};

} // namespace Render