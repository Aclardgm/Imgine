#pragma once
#ifndef ImGINE_VULKANRESSOURCES
#define ImGINE_VULKANRESSOURCES


#include "imgine_vulkaninstancebind.h"
#include <vector>
#include <vulkan/vulkan.h>



struct Imgine_VulkanTexture {
	VkExtent2D extent = { 0, 0 }; 
	VkFormat format; 
	VkImage image = VK_NULL_HANDLE;
};




struct Imgine_VulkanUniformBuffer : public Imgine_VulkanInstanceBind
{
public:
	Imgine_VulkanUniformBuffer(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}

public:
	void Create();
	void Cleanup();
	void Update(VkExtent2D extent, uint32_t imageIndex);

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
};


#endif
