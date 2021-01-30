#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) uniform mat4 model;

layout(std140, binding = 0) uniform Camera {
	mat4 camMat;
	vec3 camPos;
};

vec4 worldPos = model * vec4(pos, 1);

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outuv;

void main() {
	gl_Position = camMat * worldPos;
	outWorldPos = vec3(worldPos);
	outNormal = mat3(transpose(inverse(model))) * normal;
	outuv = uv;
}
