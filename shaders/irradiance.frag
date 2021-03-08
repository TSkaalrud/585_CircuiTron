#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(binding = 1) uniform samplerCube rectSkybox;

out vec4 outColour;

const float pi = 3.1415927;

void main() {
	vec3 normal = normalize(TexCoords);
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0; 
	for(float phi = 0.0; phi < 2.0 * pi; phi += sampleDelta) {
		for(float theta = 0.0; theta < 0.5 * pi; theta += sampleDelta) {
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			irradiance += texture(rectSkybox, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = pi * irradiance / float(nrSamples);
	outColour = vec4(irradiance, 1.0);
}