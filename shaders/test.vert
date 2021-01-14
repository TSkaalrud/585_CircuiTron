#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

layout(std140, binding = 0) uniform Camera {
	mat4 view;
	mat4 proj;
	vec3 camPos;
};

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;

void main() {
	vec4 worldPos = vec4(pos, 1);
	outWorldPos = vec3(worldPos);
	gl_Position = proj * view * worldPos;

	outNormal = normal;
}
