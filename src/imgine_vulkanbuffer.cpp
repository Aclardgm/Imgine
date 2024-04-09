#include "imgine_vulkanbuffer.h"
#include "imgine_vulkan.h"
#include "imgine_vulkanhelpers.h"

Buffer::Buffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags)
	: Imgine_VulkanInstanceBind(instance), size{size}
{
	
	VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.usage = buffer_usage;
	bufferInfo.size = size;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo memoryInfo{};
	memoryInfo.flags = flags;
	memoryInfo.usage = memory_usage;

	VmaAllocationInfo allocInfo{};
	CHECK_VK("failed create Buffer", vmaCreateBuffer(instance->allocator, &bufferInfo, &memoryInfo, &buffer, &allocation, &allocInfo))
	DEBUGVMAALLOC(instance->allocator, allocation, "TemporaryBuffer", "VMA_MEMORY_USAGE_CPU_ONLY", "");

	memory = allocInfo.deviceMemory;

}

uint8_t* Buffer::map()
{
	if (!mapped)
	{
		CHECK_VK("failed map memory",
			vmaMapMemory(instance->allocator, allocation, reinterpret_cast<void**>(&mapped_data)));
		mapped = true;
	}
	return mapped_data;
}

void Buffer::unmap()
{
	vmaUnmapMemory(instance->allocator, allocation);
}

void Buffer::update(const void* data, const size_t size, const size_t offset)
{
	update(reinterpret_cast<const uint8_t*>(data), size, offset);
}

void Buffer::update(const uint8_t* data, const size_t size, const size_t offset)
{
	map();
	std::copy(data, data + size, mapped_data + offset);
	unmap();
}

void Buffer::Cleanup()
{
	DEBUGVMADESTROY(instance->allocator, allocation);
	vmaDestroyBuffer(instance->allocator, buffer, allocation);
}
