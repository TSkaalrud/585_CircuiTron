#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

layout(location = 0) uniform vec4 albedo;

layout(std140, binding = 0) uniform Camera {
	mat4 view;
	mat4 proj;
	vec3 camPos;
};

out vec4 colour;

void main() {
	vec3 toCam = normalize(pos - camPos);
	vec3 normal = normalize(norm);
	colour = albedo * dot(toCam, normal);
}
