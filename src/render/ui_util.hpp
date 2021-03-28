#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace Render {
using namespace glm;

mat4 ui_position(
	vec2 position, vec2 size, float depth = 0, float rotation = 0, vec2 global_scale = 0.5f / vec2{1920, 1080}) {
	return scale(mat4(1.0f), {global_scale.x, global_scale.y, 1}) *
		translate(mat4(1.0f), {position.x, position.y, depth}) * rotate(mat4(1.0f), rotation, vec3{0, 0, 1}) *
		scale(mat4(1.0f), vec3{size.x, size.y, 1});
}

} // namespace Render