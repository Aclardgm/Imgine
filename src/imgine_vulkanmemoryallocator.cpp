#include "imgine_vulkanmemoryallocator.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "imgine_vulkanhelpers.h"
#include "imgine_vulkan.h"

#include <stb_image.h>

VmaAllocator createVMAllocator(
	VkInstance instance,
	VkDevice device,
	VkPhysicalDevice physicalDevice) {

	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	allocatorCreateInfo.physicalDevice = physicalDevice;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.instance = instance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

	VmaAllocator allocator;
	vmaCreateAllocator(&allocatorCreateInfo, &allocator);

	return allocator;
}


void destroyVMAllocator(VmaAllocator allocator) {
	vmaDestroyAllocator(allocator);
}




// Avoid link error for copyMemoryToAllocation function
// TODO check usage if .inl files
void TemporaryFunction()
{
	copyMappedMemorytoAllocation<stbi_uc>(nullptr, nullptr, VK_NULL_HANDLE, 0, nullptr);
	copyMappedMemorytoAllocation<const Vertex>(nullptr, nullptr, VK_NULL_HANDLE, 0,nullptr);
	copyMappedMemorytoAllocation<const uint16_t>(nullptr, nullptr, VK_NULL_HANDLE, 0, nullptr);
}

template<typename T>
void copyMappedMemorytoAllocation(Imgine_Vulkan* instance, T* src, VmaAllocation allocation, uint32_t count, void** dst)
{
	vmaMapMemory(instance->allocator, allocation, dst);
	memcpy(*dst, src, sizeof(T) * count);
	vmaUnmapMemory(instance->allocator, allocation);
}








