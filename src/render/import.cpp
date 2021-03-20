#include "model_import.hpp"

#include <assimp/Importer.hpp>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <gl.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Render {

typedef unsigned int uint;

glm::mat4 convert(const aiMatrix4x4& mat) {
	// clang-format off
	return glm::mat4 {
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3, 
		mat.a4, mat.b4, mat.c4, mat.d4
	};
	// clang-format on
}
glm::vec3 convert(const aiColor3D& col) { return glm::vec3{col.r, col.g, col.b}; }
glm::vec4 convert(const aiColor4D& col) { return glm::vec4{col.r, col.g, col.b, col.a}; }

// clang-format off
void process_node(
	const aiScene* scene, const aiNode* node, Model& model, mat4 parentTransform,
	std::vector<MeshHandle>& meshes, std::vector<MaterialHandle>& materials) {
	// clang-format on

	mat4 transform = parentTransform * convert(node->mTransformation);

	for (uint m = 0; m < node->mNumMeshes; m++) {
		model.surfaces.push_back(Model::Surface{
			.mesh = meshes[node->mMeshes[m]],
			.material = materials[scene->mMeshes[node->mMeshes[m]]->mMaterialIndex],
			.transform = transform});
	}
	for (uint c = 0; c < node->mNumChildren; c++) {
		process_node(scene, node->mChildren[c], model, transform, meshes, materials);
	}
}

std::optional<TextureHandle>
loadTexture(aiMaterial* material, aiTextureType type, unsigned int index, const aiScene* scene, Render& render) {
	aiString texturePath;
	material->GetTexture(type, index, &texturePath);
	if (texturePath.length == 0) {
		return std::optional<TextureHandle>();
	}
	const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.data);
	assert(texture);

	int x, y, channels;
	auto data =
		stbi_load_from_memory(reinterpret_cast<stbi_uc*>(texture->pcData), texture->mWidth, &x, &y, &channels, 0);
	auto tex =
		render.create_texture(x, y, channels, type == aiTextureType_DIFFUSE || type == aiTextureType_EMISSIVE, data);
	stbi_image_free(data);
	return tex;
}

Model importModel(std::string filename, Render& render) {
	Assimp::Importer importer;
	const aiScene* scene =
		importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_FlipUVs);
	assert(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode);

	std::vector<MeshHandle> meshes;
	meshes.resize(scene->mNumMeshes);
	for (uint m = 0; m < scene->mNumMeshes; m++) {
		aiMesh* importMesh = scene->mMeshes[m];
		MeshDef mesh;
		for (uint v = 0; v < importMesh->mNumVertices; v++) {
			aiVector3D pos = importMesh->mVertices[v];
			aiVector3D normal = importMesh->mNormals[v];
			aiVector3D uv = importMesh->mTextureCoords[0][v];
			MeshDef::Vertex vertex{
				.pos = {pos.x, pos.y, pos.z}, .normal = {normal.x, normal.y, normal.z}, .uv = {uv.x, uv.y}};
			mesh.verticies.push_back(vertex);
		}
		for (uint f = 0; f < importMesh->mNumFaces; f++) {
			aiFace face = importMesh->mFaces[f];
			assert(face.mNumIndices == 3);
			mesh.indicies.push_back(face.mIndices[0]);
			mesh.indicies.push_back(face.mIndices[1]);
			mesh.indicies.push_back(face.mIndices[2]);
		}
		meshes[m] = render.create_mesh(mesh);
	}

	std::vector<MaterialHandle> materials;
	materials.resize(scene->mNumMaterials);
	for (uint m = 0; m < scene->mNumMaterials; m++) {
		aiMaterial* importMaterial = scene->mMaterials[m];
		aiColor4D albedoFactor;
		importMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, albedoFactor);
		std::optional<TextureHandle> albedoTex =
			loadTexture(importMaterial, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, scene, render);
		float metalFactor;
		importMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, metalFactor);
		float roughFactor;
		importMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, roughFactor);
		std::optional<TextureHandle> metalRoughTex =
			loadTexture(importMaterial, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, scene, render);
		aiColor3D emissiveFactor;
		importMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveFactor);
		std::optional<TextureHandle> emissiveTexture =
			loadTexture(importMaterial, aiTextureType_EMISSIVE, 0, scene, render);

		materials[m] = render.create_pbr_material(MaterialPBR{
			.albedoFactor = convert(albedoFactor),
			.albedoTexture = albedoTex,
			.metalFactor = metalFactor,
			.roughFactor = roughFactor,
			.metalRoughTexture = metalRoughTex,
			.emissiveFactor = convert(emissiveFactor),
			.emissiveTexture = emissiveTexture});
	}

	Model model{.render = render};

	process_node(scene, scene->mRootNode, model, mat4(1.0f), meshes, materials);

	return model;
}

TextureHandle importSkybox(std::string filename, Render&) {
	int width, height;
	float* data = stbi_loadf(filename.c_str(), &width, &height, nullptr, 3);

	GLuint texture;

	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGB16F, width, height);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);

	stbi_image_free(data);

	return texture;
}

} // namespace Render