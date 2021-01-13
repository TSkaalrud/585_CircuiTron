#version 450 core
out vec4 colour;

layout(location = 0) uniform vec4 albedo;

void main() {
	colour = albedo;
}
