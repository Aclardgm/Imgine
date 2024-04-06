#pragma once
#ifndef ImGINE_VULKANRESSOURCES
#define ImGINE_VULKANRESSOURCES


#include <vector>
#include <vulkan/vulkan.h>
#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkanmemoryallocator.h"


struct Imgine_VulkanUniformBuffer : public Imgine_VulkanInstanceBind
{
public:
	Imgine_VulkanUniformBuffer(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}

public:
	void Create();
	void Cleanup();
	void Update(VkExtent2D extent, uint32_t imageIndex);

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VmaAllocation> uniformBuffersAllocation;
	std::vector<void*> uniformBuffersMapped;
};


#endif
