#pragma once
#ifndef ImGINE_VULKANSEMAPHORE
#define ImGINE_VULKANSEMAPHORE
#include <vulkan/vulkan.h>

#include "imgine_vulkaninstancebind.h"




struct Imgine_VulkanSemaphore : public Imgine_VulkanInstanceBind {

    Imgine_VulkanSemaphore() = delete;
    Imgine_VulkanSemaphore(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance), semaphore{ 0 } {}
    void Cleanup();
    VkSemaphore semaphore;
};

#endif