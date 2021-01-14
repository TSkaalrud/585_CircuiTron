#pragma once

#include <glm/mat4x4.hpp>
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

class Render {
  private:
	int width, height;

	uint cameraBuffer;
	float fov = 70, near = 0.1, far = 100;
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
	Render(void (*(const char*))());
	Render(const Render&) = delete;

	void resize(int width, int height);

	MeshHandle create_mesh(MeshDef);
	uint create_pbr_material(MaterialPBR);
	InstanceHandle create_instance();
	void instance_set_mesh(InstanceHandle, MeshHandle);
	void instance_set_material(InstanceHandle, MaterialHandle);

	void camera_set_pos(mat4 pos) { cameraPos = pos; }

	void run();
};

void render_test();
} // namespace Render