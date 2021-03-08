#include "group.hpp"

#include <algorithm>

namespace Render {

GroupInstance::GroupInstance(Group group, mat4 transform) : render(group.render) {
	instances.resize(group.surfaces.size());
	std::transform(group.surfaces.begin(), group.surfaces.end(), instances.begin(), [this](Group::Surface surface) {
		return Instance{render.create_instance(surface.mesh, surface.material), surface.transform};
	});
	setTransform(transform);
}

void GroupInstance::setTransform(mat4 transform) {
	for (auto i : instances) {
		render.instance_set_trans(i.handle, transform * i.baseTrans);
	}
}

} // namespace Render