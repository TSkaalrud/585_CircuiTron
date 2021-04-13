#pragma once

#include <glm/gtc/integer.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <unordered_set>
#include <vector>

namespace Render {

using namespace glm;

typedef unsigned int GLuint;
typedef int GLsizei;

struct MeshDef {
	struct Vertex {
		vec3 pos;
		vec3 normal;
		vec2 uv;
	};
	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;
};

#define REGISTER(T, collection)                                                                                        \
	std::vector<T> collection;                                                                                         \
	T##Handle register##T(T item) {                                                                                    \
		uint handle = collection.size();                                                                               \
		collection.push_back(item);                                                                                    \
		return handle;                                                                                                 \
	}

typedef uint MeshHandle;
typedef uint InstanceHandle;
typedef uint MaterialHandle;
typedef uint DirLightHandle;
typedef uint TextureHandle;
typedef uint ShaderHandle;

class Wall;
class Core {
	friend Wall;

  protected:
	template <class T> static size_t vector_size(const std::vector<T>& vec) { return sizeof(T) * vec.size(); }

	struct Camera {
		mat4 proj;
		mat4 view;
		vec3 camPos;
	};

	int width, height;

	GLuint cameraBuffer;
	float fov;
	mat4 cameraPos;

	struct Instance {
		uint model;
		uint mat;
		mat4 trans;
	};
	// REGISTER(Instance, instances)
	std::vector<Instance> instances;
	std::vector<uint> recycled_instances;

	struct Mesh {
		GLuint vao;
		uint count;
		std::vector<GLuint> buffers;
	};
	// REGISTER(Mesh, meshes)
	std::vector<Mesh> meshes;
	std::vector<uint> recycled_meshes;
	MeshHandle register_mesh(Mesh item) {
		MeshHandle handle;
		if (recycled_meshes.empty()) {
			handle = meshes.size();
			meshes.push_back(item);
		} else {
			handle = recycled_meshes.back();
			recycled_meshes.pop_back();
			meshes[handle] = item;
		}
		return handle;
	}

	struct ShaderConfig {
		uint shader;
		GLuint uniform;
		std::vector<TextureHandle> textures;
	};
	struct Material {
		std::vector<ShaderConfig> shaders;
		std::unordered_set<InstanceHandle> instances = {};
	};
	REGISTER(Material, materials);
	MaterialHandle register_material(Material mat) {
		auto handle = registerMaterial(mat);
		for (auto& shaderconf : mat.shaders) {
			shaders[shaderconf.shader].materials.emplace(handle);
		}
		return handle;
	}

	struct Shader {
		GLuint shader;
		enum Type { Opaque = 1 << 0, Depth = 1 << 1, Shadow = 1 << 2, Skybox = 1 << 4, UI = 1 << 5 };
		friend inline Type operator|(const Type lhs, const Type rhs) {
			return static_cast<Type>(static_cast<int>(lhs) | static_cast<int>(rhs));
		}
		Type type;
		std::unordered_set<MaterialHandle> materials = {};
	};
	REGISTER(Shader, shaders)

	const int lightmapSize = 4096;
	const float lightmapCoverage = 300;

	struct DirLight {
		vec3 dir;
		float _pad0;
		vec3 colour;
		float _pad1;
		mat4 shadowMapTrans;
	};
	REGISTER(DirLight, dirLights)
	GLuint dirLightBuffer, dirLightShadow;

	enum class RenderOrder { Simple, Shader, UIDepth };
	void renderScene(Shader::Type type, RenderOrder order = RenderOrder::Shader);

  public:
	Core(void (*(const char*))());
	Core(const Core&) = delete;

	void resize(int width, int height) {
		this->width = width;
		this->height = height;
	}

	MeshHandle create_mesh(MeshDef);
	void delete_mesh(MeshHandle handle);
	InstanceHandle create_instance(MeshHandle mesh, MaterialHandle mat, mat4 trans = mat4(1.0f)) {
		InstanceHandle instance;
		if (recycled_instances.empty()) {
			instance = instances.size();
			instances.push_back(Instance{});
		} else {
			instance = recycled_instances.back();
			recycled_instances.pop_back();
		}
		instance_set_mesh(instance, mesh);
		instance_set_material(instance, mat);
		instance_set_trans(instance, trans);
		return instance;
	}
	void instance_set_mesh(InstanceHandle instance, MeshHandle mesh) { instances[instance].model = mesh; }
	void instance_set_material(InstanceHandle instance, MaterialHandle mat) {
		if (instances[instance].mat != -1)
			materials[instances[instance].mat].instances.erase(instance);
		instances[instance].mat = mat;
		if (mat != -1)
			materials[mat].instances.emplace(instance);
	}
	void instance_set_trans(InstanceHandle instance, mat4 trans) { instances[instance].trans = trans; }
	void delete_instance(InstanceHandle instance) {
		instance_set_material(instance, -1);
		recycled_instances.push_back(instance);
	}

	void camera_set_pos(mat4 pos) { cameraPos = glm::inverse(pos); }
	void camera_set_fov(float degrees) { fov = radians(degrees); }

	DirLightHandle create_dir_light(vec3 colour, vec3 dir) {
		uint handle = registerDirLight(DirLight{});
		dir_light_set_colour(handle, colour);
		dir_light_set_dir(handle, dir);
		return handle;
	}
	void dir_light_set_colour(DirLightHandle handle, vec3 colour) { dirLights[handle].colour = colour; }
	void dir_light_set_dir(DirLightHandle handle, vec3 dir) {
		dirLights[handle].dir = normalize(dir);
		dirLights[handle].shadowMapTrans = ortho(
											   -lightmapCoverage, lightmapCoverage, -lightmapCoverage, lightmapCoverage,
											   -lightmapCoverage, lightmapCoverage) *
			lookAt(vec3{0, 0, 0}, -dirLights[handle].dir, vec3{0, 1, 0});
	}

	enum TextureFlags { NONE = 0, SRGB = 1 << 0, MIPMAPPED = 1 << 1, ANIOSTROPIC = 1 << 2, CLAMPED = 1 << 3 };
	friend inline TextureFlags operator|(const TextureFlags lhs, const TextureFlags rhs) {
		return static_cast<TextureFlags>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}
	TextureHandle create_texture(int width, int height, int channels, TextureFlags flags, void* data);

	void run();
};

} // namespace Render