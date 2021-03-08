#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

layout(binding = 0) uniform sampler2DArrayShadow dirLightShadowMaps;

layout(std140, binding = 0) uniform Camera {
	mat4 proj;
	mat4 view;
	vec3 camPos;
};

struct DirLight {
	vec3 dir;
	vec3 colour;
	mat4 shadowMapTrans;
};
layout(std430, binding = 1) readonly buffer DirLights {
	DirLight dirLights[];
};

layout(binding = 3) uniform samplerCube irradiance;
layout(binding = 4) uniform samplerCube reflection;
layout(binding = 5) uniform sampler2D reflectionBRDF;

layout(std140, binding = 1) uniform Material {
	vec4 albedoFactor;
	vec3 emissiveFactor;
	float metalFactor;
	float roughFactor;
	float reflectionLevels;
};
layout(binding = 6) uniform sampler2D albedoTex;
layout(binding = 7) uniform sampler2D metalRoughTex;
layout(binding = 8) uniform sampler2D emissiveTexture;

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
	return vec3((D * G) / (4 * abs(dot(normal, wi)) * abs(dot(normal, wo))));
}

vec3 fresnel(vec3 wi, vec3 f0) { // Schlick
	vec3 h = normalize(wi + wo);
	return f0 + (1 - f0) * pow(1 - abs(dot(wo, h)), 5);
}

vec3 pbr_brdf(vec3 wi) {
	vec3 F = fresnel(wi, mix(vec3(0.04), albedo.rgb, metallic));
	vec3 specular = F * specular_brdf(wi);
	vec3 diffuse = (1 - F) * albedo.rgb * (1 - 0.04) * (1 - metallic) / pi;
	return diffuse + specular;
}

vec3 light(vec3 dir, vec3 colour) {
	return pbr_brdf(dir) * colour * max(dot(dir, normal), 0.0);
}

vec3 enviroment() {
	// Code from: https://learnopengl.com/PBR/IBL/Specular-IBL
	vec3 f0 = mix(vec3(0.04), albedo.rgb, metallic);
	vec3 F = f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1 - abs(dot(wo, normal)), 5);
	vec3 diffuse = texture(irradiance, normal).rgb * (1 - F) * albedo.rgb * (1 - 0.04) * (1 - metallic);
	vec2 envBRDF = texture(reflectionBRDF, vec2(max(dot(normal, wo), 0.0), roughness)).rg;
	vec3 specular = textureLod(reflection, reflect(-wo, normal), roughness * reflectionLevels).rgb * (F * envBRDF.r + envBRDF.g);
	return diffuse + specular;
}

void main() {
	colour += enviroment();

	for (int i = 0; i < dirLights.length(); ++i) {
		vec4 shadowSample = (dirLights[i].shadowMapTrans * vec4(pos, 1));
		vec3 projCoords = shadowSample.xyz / shadowSample.w;
		projCoords = projCoords * 0.5 + 0.5; 
		float shadowDepth = texture(dirLightShadowMaps, vec4(projCoords.xy, i, projCoords.z));
		colour += light(dirLights[i].dir, dirLights[i].colour * shadowDepth);
	}
	outColour = vec4(colour, 1);
}

