#include "model_import.hpp"

#include <assimp/Importer.hpp>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
glm::vec4 convert(const aiColor4D& col) { return glm::vec4{col.r, col.g, col.b, col.a}; }

// clang-format off
void process_node(
	const aiScene* scene, const aiNode* node, Render& render,
	std::vector<MeshHandle>& meshes, std::vector<MaterialHandle>& materials) {
	// clang-format on

	for (uint m = 0; m < node->mNumMeshes; m++) {
		InstanceHandle instance = render.create_instance();
		render.instance_set_mesh(instance, meshes[node->mMeshes[m]]);
		render.instance_set_material(instance, scene->mMeshes[node->mMeshes[m]]->mMaterialIndex);
		render.instance_set_trans(instance, convert(node->mTransformation));
	}
	for (uint c = 0; c < node->mNumChildren; c++) {
		process_node(scene, node->mChildren[c], render, meshes, materials);
	}
}

TextureHandle
loadTexture(aiMaterial* material, aiTextureType type, unsigned index, const aiScene* scene, Render& render) {
	aiString texturePath;
	material->GetTexture(type, index, &texturePath);
	const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.data);
	int x, y, channels;
	auto data =
		stbi_load_from_memory(reinterpret_cast<stbi_uc*>(texture->pcData), texture->mWidth, &x, &y, &channels, 4);
	return render.create_texture(x, y, data);
}

void import_scene(std::string filename, Render& render) {
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
		aiColor4D albedoColour;
		importMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, albedoColour);
		float metal;
		importMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, metal);
		float rough;
		importMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, rough);
		TextureHandle albedoTex =
			loadTexture(importMaterial, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, scene, render);
		TextureHandle metalRoughTex =
			loadTexture(importMaterial, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, scene, render);

		materials[m] = render.create_pbr_material(MaterialPBR{
			.albedo = convert(albedoColour),
			.metal = metal,
			.rough = rough,
			.albedoTexture = albedoTex,
			.metalRoughTexture = metalRoughTex});
	}

	process_node(scene, scene->mRootNode, render, meshes, materials);
}

} // namespace Render