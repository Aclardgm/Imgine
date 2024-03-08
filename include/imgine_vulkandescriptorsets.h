#pragma once
#ifndef ImGINE_VULKANDESCRIPTORSETS
#define ImGINE_VULKANDESCRIPTORSETS
#include <vector>
#include <vulkan/vulkan.h>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkanressources.h"



struct Imgine_VulkanImageView;
struct Imgine_VulkanImageSampler;


/// <summary>
/// Store vector of VkDescriptorSetLayout
/// </summary>
struct Imgine_VulkanDescriptorsSetsLayout : public Imgine_VulkanInstanceBind {

	Imgine_VulkanDescriptorsSetsLayout(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {	}

	VkDescriptorSetLayout* createDescriptorSetLayout();
	VkDescriptorSetLayout* createDescriptorTexturedSetLayout();
    void cleanup();

	std::vector<VkDescriptorSetLayout> layouts;
};


/// <summary>
/// Store VkPipelineLayout and vector of VkDescriptorSetLayout
/// </summary>
struct Imgine_VulkanLayout : public Imgine_VulkanInstanceBind {
public:
	Imgine_VulkanLayout(Imgine_Vulkan* instance) : descriptorSetsLayout(instance), Imgine_VulkanInstanceBind(instance) {}

    void cleanup();

	Imgine_VulkanDescriptorsSetsLayout	descriptorSetsLayout;
	VkPipelineLayout			pipelineLayout;
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

	void createUniformPool();
	void createUniformTexturedPool();
	void cleanup(Imgine_VulkanDescriptorSets* dedicatedSets);
	void allocateUBODescriptorsSets(
		VkDescriptorSetLayout toAllocate, 
		Imgine_VulkanDescriptorSets* setsDst, 
		Imgine_VulkanUniformBuffer* ubos);
	void allocateUBOTexturedDescriptorsSets(
		VkDescriptorSetLayout toAllocate,
		Imgine_VulkanDescriptorSets* setsDst,
		Imgine_VulkanUniformBuffer* ubos,
		Imgine_VulkanImageView* view,
		Imgine_VulkanImageSampler* sampler);
private:
	VkDescriptorPool descriptorPool;
};









#endif