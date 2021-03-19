#version 450 core
// Code from: https://learnopengl.com/PBR/IBL/Specular-IBL

layout(location = 0) in vec3 TexCoords;

layout(binding = 1) uniform samplerCube environmentMap;

layout(location = 1) uniform float roughness;

out vec4 outColour;

const float pi = 3.1415927;

float RadicalInverse_VdC(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N) {
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
	float a = roughness * roughness;

	float phi = 2.0 * pi * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates
	vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

	// from tangent-space vector to world-space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}
void main() {
	vec3 N = normalize(TexCoords);
	vec3 R = N;
	vec3 V = R;

	const uint SAMPLE_COUNT = 1024u;
	float totalWeight = 0.0;
	vec3 prefilteredColor = vec3(0.0);
	for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
		vec2 Xi = Hammersley(i, SAMPLE_COUNT);
		vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
		vec3 L  = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0);
		if(NdotL > 0.0) {
			prefilteredColor += texture(environmentMap, L).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefilteredColor = prefilteredColor / totalWeight;

	outColour = vec4(prefilteredColor, 1.0);
}