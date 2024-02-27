#pragma once
#ifndef ImGINE_ASSETLOADER
#define ImGINE_ASSETLOADER
#include <vulkan/vulkan.h>
#include <string>

struct Imgine_Vulkan;

void transitionImageLayout(
	Imgine_Vulkan* instance,
	VkImage image,
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout);
void copyBufferToImage(
	Imgine_Vulkan* instance,
	VkBuffer buffer,
	VkImage image,
	uint32_t width,
	uint32_t height);

void createImage(
	Imgine_Vulkan* instance,
	uint32_t width,
	uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory);

void createTextureImage(
	Imgine_Vulkan* instance,
	std::string path,
	VkImage& textureImage,
	VkDeviceMemory& textureImageMemory);


void loadImage(std::string path);






#endif