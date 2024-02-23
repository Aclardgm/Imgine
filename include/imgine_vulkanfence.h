#pragma once
#ifndef ImGINE_VULKANFENCE
#define ImGINE_VULKANFENCE
#include <vulkan/vulkan.h>
#include "imgine_vulkaninstancebind.h"
#include <vector>

struct Imgine_VulkanFence : public Imgine_VulkanInstanceBind {
    Imgine_VulkanFence() = delete;
    Imgine_VulkanFence(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance), fence{ 0 } {}

    VkFence fence;
};


struct Imgine_VulkanFenceManager : public Imgine_VulkanInstanceBind {
    Imgine_VulkanFenceManager(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}

    /// <summary>
    /// Check if fences are available in freeFences, return one if so, or build new one and push it to used
    /// </summary>
    /// <returns></returns>
    Imgine_VulkanFence* GetFence();
    /// <summary>
    /// Reset given fence
    /// </summary>
    /// <param name="fence"></param>
    void ResetFence(Imgine_VulkanFence* fence);

    /// <summary>
    /// Set given fence to nullptr
    /// </summary>
    /// <param name="fence"></param>
    void ReleaseFence(Imgine_VulkanFence*& fence);

private:
    std::vector<Imgine_VulkanFence*> freeFences;
    std::vector<Imgine_VulkanFence*> usedFences;
};

#endif