#include "model_import.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

typedef unsigned int uint;

void process_node(const aiScene* scene, const aiNode* node, Render::Render& render) {
	for (uint m = 0; m < node->mNumMeshes; m++) {
		Render::InstanceHandle instance = render.create_instance();
		render.instance_set_mesh(instance, node->mMeshes[m]);
		render.instance_set_material(instance, scene->mMeshes[node->mMeshes[m]]->mMaterialIndex);
	}
	for (uint c = 0; c < node->mNumChildren; c++) {
		process_node(scene, node->mChildren[c], render);
	}
}

void import_scene(std::string filename, Render::Render& render) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
	assert(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode);

	for (uint m = 0; m < scene->mNumMeshes; m++) {
		aiMesh* importMesh = scene->mMeshes[m];
		Render::MeshDef mesh;
		for (uint v = 0; v < importMesh->mNumVertices; v++) {
			aiVector3D pos = importMesh->mVertices[v];
			Render::MeshDef::Vertex vertex{.pos = {pos.x, pos.y, pos.z}};
			mesh.verticies.push_back(vertex);
		}
		for (uint f = 0; f < importMesh->mNumFaces; f++) {
			aiFace face = importMesh->mFaces[f];
			assert(face.mNumIndices == 3);
			mesh.indicies.push_back(face.mIndices[0]);
			mesh.indicies.push_back(face.mIndices[1]);
			mesh.indicies.push_back(face.mIndices[2]);
		}
		render.create_mesh(mesh);
	}

	for (uint m = 0; m < scene->mNumMaterials; m++) {
		aiMaterial* importMaterial = scene->mMaterials[m];
		aiColor3D colour;
		importMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
		render.create_pbr_material(Render::MaterialPBR{.albedo = {colour.r, colour.g, colour.b, 1}});
	}

	process_node(scene, scene->mRootNode, render);
}
