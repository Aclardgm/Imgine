#include "imgine_vulkanfence.h"
#include "imgine_vulkan.h"

Imgine_VulkanFence* Imgine_VulkanFenceManager::GetFence() {

    Imgine_VulkanFence* fence = nullptr;
    if (freeFences.size() > 0) {
        fence = freeFences[0];
        freeFences.erase(freeFences.begin());
        usedFences.push_back(fence);
        return fence;
    }
    fence = new Imgine_VulkanFence(GetVulkanInstanceBind());
    usedFences.push_back(fence);
    return fence;
}


void Imgine_VulkanFenceManager::ResetFence(Imgine_VulkanFence* fence) {
    vkResetFences(GetVulkanInstanceBind()->GetDevice(), 1, &fence->fence);
}


void Imgine_VulkanFenceManager::ReleaseFence(Imgine_VulkanFence*& fence) {
    ResetFence(fence);

    auto it = std::find(usedFences.begin(), usedFences.end(), fence);
    if (it != usedFences.end()) {
        usedFences.erase(it);
    }
    freeFences.push_back(fence);
    fence = nullptr;
}

