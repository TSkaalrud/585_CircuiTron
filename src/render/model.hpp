#pragma once

#include "render.hpp"

namespace Render {
struct Model {
	Core& render;

	struct Surface {
		MeshHandle mesh;
		MaterialHandle material;
		mat4 transform;
	};
	std::vector<Surface> surfaces;
};

class ModelInstance {
  private:
	Core& render;

	struct Instance {
		InstanceHandle handle;
		mat4 baseTrans;
	};
	std::vector<Instance> instances;

  public:
	ModelInstance(Model group) : ModelInstance(group, mat4(1.0f)) {}
	ModelInstance(Model group, mat4 transform);
	~ModelInstance();

	ModelInstance(ModelInstance&) = delete;

	void setTransform(mat4);
};

} // namespace Render
