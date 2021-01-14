#pragma once

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

namespace Render {

using namespace glm;

struct MeshDef {
	struct Vertex {
		vec3 pos;
	};
	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;
};
struct MaterialPBR {
	vec4 albedo;
};

typedef uint MeshHandle;
typedef uint InstanceHandle;
typedef uint MaterialHandle;

class Core {
  private:
	int width, height;

	uint cameraBuffer;
	float fov;
	mat4 cameraPos;

	struct Instance {
		uint model;
		uint mat;
		mat4 trans;
	};
	std::vector<Instance> instances;

	struct Mesh {
		uint vao;
		uint count;
	};
	std::vector<Mesh> meshes;

	struct Material {
		uint shader;
		MaterialPBR pbr;
	};
	std::vector<Material> materials;

	struct Shader {
		uint shader;
	};
	std::vector<Shader> shaders;

  public:
	Core(void (*(const char*))());
	Core(const Core&) = delete;

	void resize(int width, int height) {
		this->width = width;
		this->height = height;
	}

	MeshHandle create_mesh(MeshDef);
	uint create_pbr_material(MaterialPBR);
	InstanceHandle create_instance();
	void instance_set_mesh(InstanceHandle instance, MeshHandle mesh) { instances[instance].model = mesh; }
	void instance_set_material(InstanceHandle instance, MaterialHandle mat) { instances[instance].mat = mat; }

	void camera_set_pos(mat4 pos) { cameraPos = pos; }
	void camera_set_fov(float degrees) { fov = radians(degrees); }

	void run();
};
} // namespace Render