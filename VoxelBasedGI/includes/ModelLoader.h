#pragma once
#include <memory>
#include <vector>

#include "assimp/material.h"

struct TextureData;
struct aiNode;
struct aiMaterial;
class RenderBatch;
struct aiScene;
struct aiMesh;
class Mesh3D;
class Material;

class ModelLoader
{
public:
	ModelLoader() = default;

	void LoadModel(std::string filePath, std::vector<std::shared_ptr<RenderBatch>>& batches);
private:
	std::string m_currentPath;
	std::shared_ptr<Material> AssimpToMaterial(aiMaterial* mat) const;
	uint64_t ExtractTextures(aiMaterial* mat, aiTextureType type) const;
	void ProcessNode(aiNode* node, const aiScene* scene, const std::vector<Material*>& mats) const;
	std::shared_ptr<Mesh3D> AssimpToMesh3D(const aiMesh* mesh, const aiScene* scene) const;
};
