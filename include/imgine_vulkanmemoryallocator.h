#pragma once
#ifndef ImGINE_VULKANMEMORYALLOCATOR
#define ImGINE_VULKANMEMORYALLOCATOR

#include <vulkan/vulkan.h>
#include "vma/vk_mem_alloc.h"

struct Imgine_Vulkan;

VmaAllocator createVMAllocator(
	VkInstance instance,
	VkDevice device, 
	VkPhysicalDevice physicalDevice);

void destroyVMAllocator(VmaAllocator allocator);

//
//template<typename T>
//void copyMemoryToAllocation(Imgine_Vulkan* instance, T* buffer, VmaAllocation allocation, uint32_t count);

template<typename T>
void copyMappedMemorytoAllocation(Imgine_Vulkan* instance, T* src, VmaAllocation allocation, uint32_t count,void** dst);


#endif




