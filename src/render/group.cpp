#include "group.hpp"

#include <algorithm>

namespace Render {

GroupInstance::GroupInstance(Group group, mat4 transform) : render(group.render) {
	instances.resize(group.surfaces.size());
	std::transform(group.surfaces.begin(), group.surfaces.end(), instances.begin(), [this](Group::Surface surface) {
		InstanceHandle i = render.create_instance();
		render.instance_set_mesh(i, surface.mesh);
		render.instance_set_material(i, surface.material);
		return Instance{i, surface.transform};
	});
	setTransform(transform);
}

void GroupInstance::setTransform(mat4 transform) {
	for (auto i : instances) {
		render.instance_set_trans(i.handle, transform * i.baseTrans);
	}
}

} // namespace Render