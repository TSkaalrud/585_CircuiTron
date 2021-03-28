#version 450 core

layout(location = 0) in vec2 TexCoords;

layout(binding = 3) uniform sampler2D tex;

out vec4 outColour;

void main() {
	outColour = texture(tex, normalize(TexCoords));
}