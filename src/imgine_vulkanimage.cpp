#include "imgine_vulkanimage.h"
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <assert.h>
#include "imgine_assetloader.h"
#include "imgine_vulkancommandbuffer.h"
#include "imgine_vulkan.h"
#include "imgine_vulkanressources.h"
#include "imgine_vulkanhelpers.h"
#include "imgine_vulkanmemoryallocator.h"
#include "imgine_define.h"



void createImage(
    Imgine_Vulkan* instance,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage* outImage,
    VmaAllocation* outAllocation) {

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.requiredFlags = properties;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;

    vmaCreateImage(instance->allocator, &imageInfo, &allocCreateInfo, outImage, outAllocation, nullptr);
    DEBUGVMAALLOC(instance->allocator, *outAllocation, "Image", "VK_SHARING_MODE_EXCLUSIVE", "VMA_MEMORY_USAGE_AUTO");
}


VkImageView createImageView(Imgine_Vulkan* instance, VkImage image, VkFormat format)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    
    VkImageView imageView;


    CHECK_VK(
        "failed to create image view!",
        vkCreateImageView(instance->GetDevice(), &viewInfo, nullptr, &imageView)

    )

    //if (vkCreateImageView(instance->GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to create image view!");
    //}

    return imageView;
}

VkImageView createImageView(Imgine_Vulkan* instance, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;

    CHECK_VK(
        "failed to create image view!",
        vkCreateImageView(instance->GetDevice(), &viewInfo, nullptr, &imageView)
    )

    //if (vkCreateImageView(instance->GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to create image view!");
    //}

    return imageView;
}
void createSampler(Imgine_Vulkan* instance, VkSampler* sampler) {

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(instance->GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    CHECK_VK(
        "failed to create texture sampler!",
        vkCreateSampler(instance->GetDevice(), &samplerInfo, nullptr, sampler)
    )

    //if (vkCreateSampler(instance->GetDevice(), &samplerInfo, nullptr, sampler) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to create texture sampler!");
    //}
}

void destroyImage(Imgine_Vulkan* instance, VkImage image, VmaAllocation allocation)
{
    DEBUGVMADESTROY(instance->allocator, allocation);
    vmaDestroyImage(instance->allocator, image, allocation);
}

Imgine_Texture& Imgine_TextureRef::GetTexture()
{
#ifdef _DEBUG
    assert(ID != FALSE_ID);
#endif
    return Imgine_AssetLoader::GetInstance()->loadedTextures[ID];
}
Imgine_Texture& Imgine_TextureRef::GetTexture() const
{
    #ifdef _DEBUG
        assert(ID != FALSE_ID);
    #endif
        return Imgine_AssetLoader::GetInstance()->loadedTextures[ID];
}
void Imgine_Texture::Cleanup(Imgine_Vulkan* instance)
{
    image.Cleanup(instance);
}

void Imgine_VulkanImage::Cleanup(Imgine_Vulkan* instance)
{
    destroyImage(instance, image, allocation);
}

void Imgine_VulkanImageView::Cleanup(Imgine_Vulkan* instance)
{
    vkDestroyImageView(instance->device, view,nullptr);
}

void Imgine_VulkanImageSampler::Cleanup(Imgine_Vulkan* instance)
{
    vkDestroySampler(instance->device, sampler, nullptr);
}

