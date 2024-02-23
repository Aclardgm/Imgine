#pragma once
#ifndef ImGINE_VULKANIMAGE
#define ImGINE_VULKANIMAGE
#include <vulkan/vulkan.h>


struct Imgine_VulkanImage {

    VkImage image;
    VkDeviceMemory imageMemory;
};

struct Imgine_VulkanImageView {
    Imgine_VulkanImage* image;
    VkImageView view;
};


#endif