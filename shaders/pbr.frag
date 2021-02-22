#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

layout(binding = 0) uniform sampler2DArrayShadow dirLightShadowMaps;

layout(std140, binding = 0) uniform Camera {
	mat4 camMat;
	vec3 camPos;
};

layout(std140, binding = 1) uniform Material {
	vec4 albedoFactor;
	vec3 emissiveFactor;
	float metalFactor;
	float roughFactor;
};
layout(binding = 5) uniform sampler2D albedoTex;
layout(binding = 6) uniform sampler2D metalRoughTex;
layout(binding = 7) uniform sampler2D emissiveTexture;

struct DirLight {
	vec3 dir;
	vec3 colour;
	mat4 shadowMapTrans;
};
layout(std430, binding = 1) readonly buffer DirLights {
	DirLight dirLights[];
};

out vec4 outColour;

vec4 albedo = albedoFactor * texture(albedoTex, uv);
vec4 metalRough = texture(metalRoughTex, uv);
float metallic = metalFactor * metalRough.b;
float roughness = roughFactor * metalRough.g;
vec3 emissive = emissiveFactor * texture(emissiveTexture, uv).rgb;

vec3 wo = normalize(camPos - pos);
vec3 normal = normalize(norm);
vec3 colour = emissive;

const float pi = 3.1415927;

vec3 diffuse_brdf(vec3 wi) { // Lambert
	return albedo.rgb / pi;
}

float alpha2 = pow(roughness, 4);
float normal_dist(vec3 wi) { // GGX / Trowbridge-Reitz
	vec3 h = normalize(wi + wo);
	float ndoth = dot(normal, h);
	return alpha2 / (pi * pow((ndoth * ndoth * (alpha2 - 1) + 1), 2));
}

float k = ((roughness + 1) * (roughness + 1)) / 8;
float geom_atten_part (vec3 v) {
	float ndotv = dot(normal, v);
	return ndotv / (ndotv * (1 - k) + k);
}
float geom_atten (vec3 wi) { // Schlick
	return geom_atten_part(wo) * geom_atten_part(wi);
}

vec3 specular_brdf(vec3 wi) { // Cook-Torrance
	float D = normal_dist(wi);
	float G = geom_atten(wi);
	return vec3((D * G) / (4 * dot(normal, wi) * dot(normal, wo)));
}

vec3 fresnel(vec3 wi, vec3 f0) { // Schlick
	vec3 h = normalize(wi + wo);
	return f0 + (1 - f0) * pow(1 - abs(dot(wo, h)), 5);
}

vec3 fresnel_mix(vec3 wi, vec3 layer, vec3 base) {
	return mix(base, layer, fresnel(wi, vec3(0.04)));
}

vec3 conductor_fresnel(vec3 wi, vec3 brdf, vec3 f0) {
	return brdf * fresnel(wi, f0);
}

vec3 metal_brdf(vec3 wi) {
	return conductor_fresnel(wi, specular_brdf(wi), albedo.rgb);
}

vec3 dielectric_brdf(vec3 wi) {
	return fresnel_mix(wi, specular_brdf(wi), diffuse_brdf(wi));
}

vec3 pbr_brdf(vec3 wi) {
	return mix(dielectric_brdf(wi), metal_brdf(wi), metallic);
}

vec3 light(vec3 dir, vec3 colour) {
	return pbr_brdf(dir) * colour * max(dot(dir, normal), 0.0);
}

void main() {
	for (int i = 0; i < dirLights.length(); ++i) {
		vec4 shadowSample = (dirLights[i].shadowMapTrans * vec4(pos, 1));
		vec3 projCoords = shadowSample.xyz / shadowSample.w;
		projCoords = projCoords * 0.5 + 0.5; 
		float shadowDepth = texture(dirLightShadowMaps, vec4(projCoords.xy, i, projCoords.z));
		colour += light(dirLights[i].dir, dirLights[i].colour * shadowDepth);
	}
	outColour = vec4(colour, 1);
}

