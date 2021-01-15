#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

layout(location = 1) uniform vec4 albedo;

layout(std140, binding = 0) uniform Camera {
	mat4 view;
	mat4 proj;
	vec3 camPos;
};

struct Light {
	vec3 dir;
	vec3 colour;
};
layout(std430, binding = 1) readonly buffer DirLights {
	Light dirLights[];
};

out vec4 outColour;

vec3 light(Light light, vec3 normal) {
	return albedo.rgb * light.colour * dot(light.dir, normal);
}

void main() {
	vec3 toCam = normalize(pos - camPos);
	vec3 normal = normalize(norm);
	vec3 colour = vec3(0, 0, 0);
	for (int i = 0; i < dirLights.length(); ++i) {
		colour += light(dirLights[i], normal);
	}
	outColour = vec4(colour, 1);
}

