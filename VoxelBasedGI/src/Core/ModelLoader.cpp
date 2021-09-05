#include "ModelLoader.h"
#include "Mesh3D.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Program.h"
#include "RenderBatch.h"

void ModelLoader::LoadModel(std::string filePath, std::vector<std::shared_ptr<RenderBatch>>& batches)
{
	m_currentPath = filePath.substr(0, filePath.find_last_of('/'));
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
	                                         aiProcess_Triangulate |
	                                         aiProcess_GenSmoothNormals |
	                                         aiProcess_CalcTangentSpace |
	                                         aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes |
	                                         aiProcess_OptimizeMeshes);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: ASSIMP model at " << filePath << " couldn't be loaded\n" << importer.GetErrorString();
		return;
	}

	std::vector<Material*> mats;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		const auto& ownMat = AssimpToMaterial(scene->mMaterials[i]);
		const auto batch = std::make_shared<RenderBatch>(ownMat);
		mats.push_back(ownMat.get());
		batches.push_back(batch);
	}
	ProcessNode(scene->mRootNode, scene, mats);

	for (auto& batch : batches)
	{
		batch->CollectGeometry();
	}

	// No need to keep the empty batches
	batches.erase(
		std::remove_if(batches.begin(), batches.end(),
		               [](const auto& batch) { return batch->GetNumOfDrawCalls() == 0; }),
		batches.end());
}

std::shared_ptr<Material> ModelLoader::AssimpToMaterial(aiMaterial* mat) const
{
	using namespace MaterialUtilities;
	Shader normalShader("Shaders/PhongVertexColor.vert", "Shaders/DefaultDeferredFrag.frag");
	std::vector<UniformAttribute> uniforms;
	std::vector<ShaderAttribute> tmp2;
	tmp2.push_back(ShaderAttribute::TEXCOORDS);
	tmp2.push_back(ShaderAttribute::NORMALS);
	tmp2.push_back(ShaderAttribute::TANGENTSPACE);

	aiColor3D diffuse;
	if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
	{
		std::cout << "Couldn't load diffuse color of Material\n";
	}
	aiColor3D ambient;
	if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
	{
		std::cout << "Couldn't load ambient color of Material\n";
	}
	aiColor3D specular;
	if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_SPECULAR, specular))
	{
		std::cout << "Couldn't load specular color of Material\n";
	}
	aiColor3D shininess;
	if (AI_SUCCESS != mat->Get(AI_MATKEY_SHININESS, shininess))
	{
		std::cout << "Couldn't load shininess of Material\n";
	}
	aiColor3D emission;
	if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_EMISSIVE, emission))
	{
		std::cout << "Couldn't load shininess of Material\n";
	}

	Properties props(glm::vec3{ambient.r, ambient.g, ambient.b},
	                 glm::vec3{diffuse.r, diffuse.g, diffuse.b},
	                 glm::vec3{emission.r * 10.0f, emission.g * 10.0f, emission.b * 10.0f},
	                 shininess.r < 0.01f ? 2 : shininess.r);

	std::shared_ptr<TextureData> tmp = std::make_unique<TextureData>();
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		tmp->m_albedo = ExtractTextures(mat, aiTextureType_DIFFUSE);
		uniforms.push_back(UniformAttribute::ALBEDO);
	}
	if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0)
	{
		tmp->m_normalMap = ExtractTextures(mat, aiTextureType_HEIGHT);
		uniforms.push_back(UniformAttribute::NORMALMAP);
	}
	if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		tmp->m_specularMap = ExtractTextures(mat, aiTextureType_SPECULAR);
		uniforms.push_back(UniformAttribute::SPECULARMAP);
	}

	return std::make_shared<Material>(MaterialPreset::Default, normalShader, Program::m_materialIndex++, props, tmp,
	                                  tmp2, uniforms);
}

uint64_t ModelLoader::ExtractTextures(aiMaterial* mat, aiTextureType type) const
{
	aiString str;
	mat->GetTexture(type, 0, &str);
	auto c = m_currentPath + '/' + std::string(str.C_Str());

	return TextureManager::Get()->GetTextureHandle(c.data(), type == aiTextureType_DIFFUSE);
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, const std::vector<Material*>& mats) const
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const auto* mesh = scene->mMeshes[node->mMeshes[i]];
		auto mesh3D = AssimpToMesh3D(mesh, scene);

		// Hardcoded scaling for Cornell Box to look good
		mesh3D->AdditiveTranslation(glm::vec3(1.1, 0, 1.1));

		GeometryManager::Get()->AddGeometry(mesh3D, *mats.at(mesh->mMaterialIndex));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, mats);
	}
}

std::shared_ptr<Mesh3D> ModelLoader::AssimpToMesh3D(const aiMesh* mesh, const aiScene* scene) const
{
	auto myMesh = std::make_shared<Mesh3D>();
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		myMesh->AddPosition({mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
		if (mesh->HasNormals())
			myMesh->AddNormal({mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
		if (mesh->HasTextureCoords(0))
			myMesh->AddTexCoord({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
		else
			myMesh->AddTexCoord({0.0f, 0.0f});
		if (mesh->HasTangentsAndBitangents())
		{
			myMesh->AddTangentsAndBitangents({mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z},
			                                 {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z});
		}
		if (mesh->HasVertexColors(0))
			myMesh->AddVertexColor({mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b});
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			myMesh->AddIndex(face.mIndices[j]);
		}
	}
	// Uncomment to rotate objects with mouse (Only to show how VoxelGI breaks down with dynamic objects and no optimization!)
	//myMesh->ConnectToMouseInput();

	return myMesh;
}
