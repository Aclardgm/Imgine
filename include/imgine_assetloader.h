#pragma once
#ifndef ImGINE_ASSETLOADER
#define ImGINE_ASSETLOADER
#include <vulkan/vulkan.h>
#include <string>
#include <array>
#include <optional>
#include "imgine_vulkanmemoryallocator.h"
#include "imgine_mesh.h"


struct Imgine_Vulkan;
enum Imgine_TextureTypes;


void loadImage(Imgine_Vulkan* instance, std::string path, VkImage* image, VmaAllocation* allocation);

void loadMesh(Imgine_Vulkan* instance, std::string path, VkImage* image, VmaAllocation* allocation);



class Imgine_AssetLoader {


private:
	Imgine_AssetLoader() {}
	Imgine_AssetLoader(const Imgine_AssetLoader& al) = delete;
public:
	enum TextureTypes {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		HEIGHT
	};

	inline static const std::array<std::string, 4> TextureTypeNames = {
		"texture_diffuse",
		"texture_specular",
		"texture_normal",
		"texture_height"
	};



	static Imgine_AssetLoader* GetInstance() {
		static Imgine_AssetLoader instance;
		return &instance;
	}

	std::optional<Imgine_TextureRef> GetTextureRef(const char* path);

	//Check if texture exist, load it if not and return corresponding ref
	Imgine_TextureRef loadTexture(Imgine_Vulkan* instance, const char* path, std::string typeName);
	//Check if texture exist, load it if not and return corresponding ref
	Imgine_TextureRef loadTexture(Imgine_Vulkan* instance, const char* path, TextureTypes type);
	Imgine_MeshRef loadModel(const char* path);


	std::vector<Imgine_Texture> loadedTextures;
	std::vector<Imgine_Mesh> loadedMeshes;

	void Cleanup(Imgine_Vulkan* instance);
};

#endif