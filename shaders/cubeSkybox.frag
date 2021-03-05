#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(binding = 5) uniform samplerCube rectSkybox;

out vec4 outColour;

void main() {
	outColour = texture(rectSkybox,normalize(TexCoords));
}