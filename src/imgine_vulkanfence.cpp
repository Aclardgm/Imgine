#include "imgine_vulkanfence.h"
#include "imgine_vulkan.h"

Imgine_VulkanFence* Imgine_VulkanFenceManager::getFence() {

    Imgine_VulkanFence* fence = nullptr;
    if (freeFences.size() > 0) {
        fence = freeFences[0];
        freeFences.erase(freeFences.begin());
        usedFences.push_back(fence);
        return fence;
    }
    fence = new Imgine_VulkanFence(getVulkanInstanceBind());
    usedFences.push_back(fence);
    return fence;
}


void Imgine_VulkanFenceManager::resetFence(Imgine_VulkanFence* fence) {
    vkResetFences(getVulkanInstanceBind()->GetDevice(), 1, &fence->fence);
}


void Imgine_VulkanFenceManager::releaseFence(Imgine_VulkanFence*& fence) {
    resetFence(fence);

    auto it = std::find(usedFences.begin(), usedFences.end(), fence);
    if (it != usedFences.end()) {
        usedFences.erase(it);
    }
    freeFences.push_back(fence);
    fence = nullptr;
}

