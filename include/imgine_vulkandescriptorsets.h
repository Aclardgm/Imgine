#pragma once
#ifndef ImGINE_VULKANDESCRIPTORSETS
#define ImGINE_VULKANDESCRIPTORSETS
#include <vector>
#include <vulkan/vulkan.h>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkanressources.h"

/// <summary>
/// Store vector of VkDescriptorSetLayout
/// </summary>
struct Imgine_VulkanDescriptorsSetsLayout : public Imgine_VulkanInstanceBind {

	Imgine_VulkanDescriptorsSetsLayout(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {	}

	VkDescriptorSetLayout* CreateDescriptorSetLayout();
    void Cleanup();

	std::vector<VkDescriptorSetLayout> layouts;
};


/// <summary>
/// Store VkPipelineLayout and vector of VkDescriptorSetLayout
/// </summary>
struct Imgine_VulkanLayout : public Imgine_VulkanInstanceBind {
public:
	Imgine_VulkanLayout(Imgine_Vulkan* instance) : DescriptorSetLayout(instance), Imgine_VulkanInstanceBind(instance) {}
	Imgine_VulkanDescriptorsSetsLayout	DescriptorSetLayout;
	VkPipelineLayout			PipelineLayout;
    void Cleanup();
};


/// <summary>
/// Store vector of VkDescriptorSet
/// </summary>
struct Imgine_VulkanDescriptorSets : public Imgine_VulkanInstanceBind {
	Imgine_VulkanDescriptorSets(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}
public:
	std::vector<VkDescriptorSet> descriptorSets;
};

/// <summary>
/// Store descriptor pool 
/// </summary>
struct Imgine_VulkanDescriptorPool : public Imgine_VulkanInstanceBind
{
public:
	Imgine_VulkanDescriptorPool(Imgine_Vulkan* instance) :  Imgine_VulkanInstanceBind(instance) {}
	void CreateUniformPool();
	void Cleanup(Imgine_VulkanDescriptorSets* dedicatedSets);
	void AllocateUBODescriptorsSets(VkDescriptorSetLayout toAllocate, Imgine_VulkanDescriptorSets* setsDst, Imgine_VulkanUniformBuffer* ubos);
private:
	VkDescriptorPool descriptorPool;
};









#endif