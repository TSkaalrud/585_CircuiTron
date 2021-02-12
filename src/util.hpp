#pragma once

#include <foundation/PxMat44.h>
#include <foundation/PxTransform.h>
#include <glm/mat4x4.hpp>

glm::mat4 convertTransform(physx::PxTransform& t) {
	physx::PxMat44 m(t);
	return *reinterpret_cast<glm::mat4*>(&m);
}