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

vec3 camDir = normalize(camPos - pos);
vec3 normal = normalize(norm);
vec3 colour = vec3(0, 0, 0);

vec3 light(Light light) {
	return albedo.rgb * light.colour * max(dot(light.dir, normal),0.0);
}
void main() {
	for (int i = 0; i < dirLights.length(); ++i) {
		colour += light(dirLights[i]);
	}
	outColour = vec4(colour, 1);
}

