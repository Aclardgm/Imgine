#pragma once
#ifndef ImGINE_VULKANBUFFER
#define ImGINE_VULKANBUFFER
#include <vulkan/vulkan.h>
#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkanmemoryallocator.h"

struct Buffer : public Imgine_VulkanInstanceBind
{
	Buffer(
		Imgine_Vulkan* instance,
		VkDeviceSize size,
		VkBufferUsageFlags buffer_usage,
		VmaMemoryUsage memory_usage,
		VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
	);

	const VkBuffer* get() const
	{
		return &buffer;
	}

	VmaAllocation getAllocation()
	{
		return allocation;
	}

	VkDeviceMemory getMemory()
	{
		return memory;
	}

	VkDeviceSize getSize()
	{
		return size;
	}

	/// <summary>
	/// Map memory if not already mapped
	/// </summary>
	/// <returns></returns>
	uint8_t* map();

	void unmap();

	void update(const void* data, const size_t size, const size_t offset);
	void update(const uint8_t* data, const size_t size, const size_t offset);

	void Cleanup();

	VkBuffer buffer;
	VmaAllocation allocation;
	VkDeviceMemory memory;
	VkDeviceSize size;
	uint8_t* mapped_data{ nullptr };
	bool mapped = false;
};





#endif