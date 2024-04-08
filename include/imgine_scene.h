#pragma once
#ifndef ImGINE_SCENE
#define ImGINE_SCENE

#include <string>
#include <array>
#include "imgine_assimp.h"
#include "imgine_assetloader.h"
#include "imgine_vulkaninstancebind.h"

enum SceneFiles
{
	VikingRoom,
	Sponza
};


struct SceneImport
{
	std::string targetFile;
	unsigned int flags;
};


const std::array<SceneImport,2> scenesImports =
{
	{
		{
			"models/viking_room.obj",
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs |
			aiProcess_SortByPType
		},
		{
			"models/sponza/sponza.obj",
			aiProcessPreset_TargetRealtime_MaxQuality
		}
	}
};


struct Imgine_Scene : public Imgine_VulkanInstanceBind
{
	Imgine_Scene(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}
	Imgine_Scene(Imgine_Scene& scene) : models(scene.models), Imgine_VulkanInstanceBind(scene.getVulkanInstanceBind()) {}

	Imgine_Scene& operator=(Imgine_Scene&& other)
	{
		models = std::move(other.models);
		return *this;
	}


	void loadScene(SceneFiles targetScene);

	std::vector<Imgine_VulkanModel> models;
};




#endif