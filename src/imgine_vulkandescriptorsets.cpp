#include "imgine_vulkandescriptorsets.h"
#include "imgine_vulkan.h"


VkDescriptorSetLayout* Imgine_VulkanDescriptorsSetsLayout::CreateDescriptorSetLayout()
{
    layouts.push_back(VkDescriptorSetLayout());

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(GetVulkanInstanceBind()->GetDevice(), &layoutInfo, nullptr, &layouts[layouts.size() - 1]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return &layouts[layouts.size() - 1];
}


void Imgine_VulkanDescriptorPool::CreateUniformPool()
{
    VkDevice device = GetVulkanInstanceBind()->GetDevice();
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}


void Imgine_VulkanDescriptorPool::AllocateUBODescriptorsSets(
    VkDescriptorSetLayout toAllocate, 
    Imgine_VulkanDescriptorSets* setsDst,
    Imgine_VulkanUniformBuffer* ubos)
{
    VkDevice device = GetVulkanInstanceBind()->GetDevice();

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, toAllocate);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    setsDst->descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device, &allocInfo, setsDst->descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ubos->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = setsDst->descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}





void Imgine_VulkanDescriptorsSetsLayout::Cleanup()
{
	for (auto lay : layouts)
	{
		vkDestroyDescriptorSetLayout(GetVulkanInstanceBind()->GetDevice(), lay, nullptr);
	}
    layouts.clear();
}
void Imgine_VulkanDescriptorPool::Cleanup(Imgine_VulkanDescriptorSets* dedicatedSets)
{
    vkDestroyDescriptorPool(GetVulkanInstanceBind()->GetDevice(), descriptorPool, nullptr);
}
void Imgine_VulkanLayout::Cleanup()
{
	VkDevice device = GetVulkanInstanceBind()->GetDevice();
	DescriptorSetLayout.Cleanup();
}

