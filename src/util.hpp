#pragma once

#include <foundation/PxTransform.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

glm::mat4 convertTransform(physx::PxTransform t) {
	return glm::translate(glm::toMat4(glm::quat{t.q.w, t.q.x, t.q.y, t.q.z}), glm::vec3{t.p.x, t.p.y, t.p.z});
}