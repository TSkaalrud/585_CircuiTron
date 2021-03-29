#version 450 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

layout(location = 0) uniform mat4 transform;

layout(location = 0) out vec2 outuv;

void main() {
	gl_Position = transform * vec4(pos, 0.0, 1.0);
	outuv = uv;
}