#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(binding = 5) uniform sampler2D rectSkybox;

out vec4 outColour;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main() {
	outColour = texture(rectSkybox,SampleSphericalMap(normalize(TexCoords)));
}