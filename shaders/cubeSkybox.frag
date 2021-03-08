#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(binding = 3) uniform samplerCube cubeSkybox;

out vec4 outColour;

void main() {
	outColour = texture(cubeSkybox, normalize(TexCoords));
}