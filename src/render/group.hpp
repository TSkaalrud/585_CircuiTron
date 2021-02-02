#pragma once

#include "render.hpp"

namespace Render {
struct Group {
	Core& render;

	struct Surface {
		MeshHandle mesh;
		MaterialHandle material;
		mat4 transform;
	};
	std::vector<Surface> surfaces;
};

class GroupInstance {
  private:
	Core& render;

	struct Instance {
		InstanceHandle handle;
		mat4 baseTrans;
	};
	std::vector<Instance> instances;

  public:
	GroupInstance(Group group) : GroupInstance(group, mat4(1.0f)) {}
	GroupInstance(Group group, mat4 transform);

	void setTransform(mat4);
};
} // namespace Render
