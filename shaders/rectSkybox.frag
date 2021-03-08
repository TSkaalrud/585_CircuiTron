#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(binding = 3) uniform sampler2D rectSkybox;

out vec4 outColour;

const float pi = 3.1415927;

vec2 equirect(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= vec2(-1.0 / 2.0 / pi, -1.0 / pi);
	uv += 0.5;
	return uv;
}

void main() {
	outColour = texture(rectSkybox, equirect(normalize(TexCoords)));
}