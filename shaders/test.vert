#version 450 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform Camera {
	mat4 view;
	mat4 proj;
};

void main() {
	gl_Position = proj * view * vec4(pos, 1);
}
