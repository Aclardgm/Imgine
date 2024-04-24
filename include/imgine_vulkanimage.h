#pragma once
#ifndef ImGINE_VULKANIMAGE
#define ImGINE_VULKANIMAGE
#include <vulkan/vulkan.h>
#include <string>

#include "imgine_define.h"
#include "imgine_vulkanmemoryallocator.h"
#include "imgine_stb.h"


struct Imgine_Texture;
struct Imgine_TextureRef;
struct Imgine_VulkanImage;
struct Imgine_VulkanImageView;
struct Imgine_VulkanImageSampler;


//Basic vulkan image = image data + allocation
struct Imgine_VulkanImage {
	VkImage image;
	VmaAllocation allocation;
	VkFormat format = VK_FORMAT_UNDEFINED;
	void Cleanup(Imgine_Vulkan* instance);
};


// Loaded texture asset, allocation and data + identification (path and type)
struct Imgine_Texture {
	Imgine_VulkanImage image;
	std::string type;
	std::string path;

	VkImage GetImage() {
		return image.image;
	}
	VmaAllocation GetAllocation() {
		return image.allocation;
	}

	void Cleanup(Imgine_Vulkan* instance);
};


//Id to specific loaded asset in AssetLoader
struct Imgine_TextureRef {
	
	unsigned int ID = FALSE_ID;
public:
	Imgine_Texture& GetTexture();
	Imgine_Texture& GetTexture() const;
};

struct Imgine_VulkanImageView {
	Imgine_VulkanImageView(Imgine_TextureRef ref = {}) : imageRef(ref) {}

	void Cleanup(Imgine_Vulkan* instance);

	Imgine_TextureRef imageRef;
	VkImageView view;
};

struct Imgine_VulkanImageSampler {

    Imgine_VulkanImageSampler() {}
	void Cleanup(Imgine_Vulkan* instance);
    VkSampler sampler;
};




void transitionImageLayout(
	Imgine_Vulkan* instance,
	VkImage image,
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout);


void transitionImageLayout(
	Imgine_Vulkan* instance,
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspectFlags,
	VkImageLayout oldLayout,
	VkImageLayout newLayout);
//
//void copyBufferToImage(
//	Imgine_Vulkan* instance,
//	VkBuffer buffer,
//	VkImage image,
//	uint32_t width,
//	uint32_t height);

void createImage(
	Imgine_Vulkan* instance,
	uint32_t width,
	uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage* outImage,
	VmaAllocation* outAllocation);
//
//void createTextureImage(
//	Imgine_Vulkan* instance,
//	VkDeviceSize imageSize,
//	stbi_uc* pixels,
//	int width,
//	int height,
//	VkImage* textureImage,
//	VmaAllocation* allocation);


VkImageView createImageView(Imgine_Vulkan* instance, VkImage image, VkFormat format);
VkImageView createImageView(Imgine_Vulkan* instance, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
void createSampler(Imgine_Vulkan* instance, VkSampler* sampler);


void destroyImage(Imgine_Vulkan* instance, VkImage image, VmaAllocation allocation);


#endif