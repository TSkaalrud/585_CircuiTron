#version 450 core
layout(location = 0) in vec3 pos;

layout(location = 0) uniform mat4 model;

layout(std140, binding = 0) uniform Camera {
	mat4 proj;
	mat4 view;
	vec3 camPos;
};

layout(location = 0) out vec3 outPos;

void main() {
	outPos = pos;
	gl_Position = (proj * mat4(mat3(view)) * vec4(pos, 1)).xyww;
}