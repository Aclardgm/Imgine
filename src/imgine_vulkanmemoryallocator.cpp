#include "imgine_vulkanmemoryallocator.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "imgine_vulkanhelpers.h"
#include "imgine_vulkan.h"


VmaAllocator createVMAllocator(
	VkInstance instance,
	VkDevice device,
	VkPhysicalDevice physicalDevice) {

	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	allocatorCreateInfo.vulkanApiVersion = VK_MAKE_VERSION(1,3,231);
	allocatorCreateInfo.physicalDevice = physicalDevice;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.instance = instance;
	allocatorCreateInfo.pVulkanFunctions = NULL;// &vulkanFunctions;

	VmaAllocator allocator;
	vmaCreateAllocator(&allocatorCreateInfo, &allocator);

	return allocator;
}


void destroyVMAllocator(VmaAllocator allocator) {
	vmaDestroyAllocator(allocator);
}




// Avoid link error for copyMemoryToAllocation function
// TODO check usage of .inl files
void TemporaryFunction()
{
	copyMappedMemorytoAllocation<stbi_uc>(nullptr, nullptr, VK_NULL_HANDLE, 0, nullptr);
	copyMappedMemorytoAllocation<const Imgine_Vertex>(nullptr, nullptr, VK_NULL_HANDLE, 0,nullptr);
	copyMappedMemorytoAllocation<Imgine_Vertex>(nullptr, nullptr, VK_NULL_HANDLE, 0, nullptr);
	copyMappedMemorytoAllocation<const uint32_t>(nullptr, nullptr, VK_NULL_HANDLE, 0, nullptr);
	copyMappedMemorytoAllocation<uint32_t>(nullptr, nullptr, VK_NULL_HANDLE, 0, nullptr);

	copyMappedMemorytoAllocation<stbi_uc>(nullptr, nullptr, VK_NULL_HANDLE,  nullptr,0);
	copyMappedMemorytoAllocation<const Imgine_Vertex>(nullptr, nullptr, VK_NULL_HANDLE,  nullptr,0);
	copyMappedMemorytoAllocation<Imgine_Vertex>(nullptr, nullptr, VK_NULL_HANDLE, nullptr, 0);
	copyMappedMemorytoAllocation<const uint32_t>(nullptr, nullptr, VK_NULL_HANDLE, nullptr, 0);
	copyMappedMemorytoAllocation<uint32_t>(nullptr, nullptr, VK_NULL_HANDLE,  nullptr,0);
}

template<typename T>
void copyMappedMemorytoAllocation(Imgine_Vulkan* instance, T* src, VmaAllocation allocation, uint32_t count, void** dst)
{
	vmaMapMemory(instance->allocator, allocation, dst);
	memcpy(*dst, src, sizeof(T) * count);
	vmaUnmapMemory(instance->allocator, allocation);
}
template<typename T>
void copyMappedMemorytoAllocation(Imgine_Vulkan* instance, T* src, VmaAllocation allocation,void** dst,size_t size)
{
	vmaMapMemory(instance->allocator, allocation, dst);
	memcpy(*dst, src, size);
	vmaUnmapMemory(instance->allocator, allocation);
}









