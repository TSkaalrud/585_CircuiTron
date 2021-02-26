#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(binding = 5) uniform sampler2D rectSkybox;

out vec4 outColour;

vec2 equirect(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= vec2(-0.1591, -0.3183);
	uv += 0.5;
	return uv;
}

void main() {
	outColour = texture(rectSkybox, equirect(normalize(TexCoords)));
}