#pragma once

#ifndef ImGINE_ASSIMP
#define ImGINE_ASSIMP

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <filesystem>

#include "imgine_assetloader.h"

struct Imgine_Mesh;
struct Imgine_TextureRef;
struct Imgine_Vulkan;

bool AssimpImportScene(Imgine_Vulkan* instance, const std::string& pFile, std::vector<Imgine_Mesh>& meshes, unsigned int flags);

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Imgine_TextureRef> loadMaterialTextures(
    Imgine_Vulkan* instance, aiMaterial* mat, aiTextureType type, std::filesystem::path path, std::string typeName);

std::vector<Imgine_TextureRef> loadMaterialTextures(
    Imgine_Vulkan* instance, aiMaterial* mat, aiTextureType type, std::filesystem::path path, Imgine_AssetLoader::TextureTypes typeEnum);


void processNode(Imgine_Vulkan* instance, aiNode* node, const aiScene* scene, std::filesystem::path path, std::vector<Imgine_Mesh>& meshes);

Imgine_Mesh processMesh(Imgine_Vulkan* instance, aiMesh* mesh, std::filesystem::path path, const aiScene* scene);




#endif