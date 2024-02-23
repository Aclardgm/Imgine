#pragma once
#ifndef ImGINE_VULKANCOMMANDBUFFER
#define ImGINE_VULKANCOMMANDBUFFER
#include <vulkan/vulkan.h>
#include <vector>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkansemaphore.h"


struct Imgine_VulkanRenderPass;
struct Imgine_SwapChain;


struct Imgine_CommandBuffer : public Imgine_VulkanInstanceBind {
public:
    void Begin();
    void End();
    void BeginRenderPass(Imgine_VulkanRenderPass* renderPass, Imgine_SwapChain* swapChain, uint32_t imageIndex);
    void EndRenderPass();
    VkCommandBuffer commandBuffer;
};



struct Imgine_CommandBufferPool : public Imgine_VulkanInstanceBind {
public:
    Imgine_CommandBufferPool() = delete;
    Imgine_CommandBufferPool(Imgine_Vulkan* instance) :Imgine_VulkanInstanceBind(instance) {}



    Imgine_CommandBuffer* Create();
    void Create(VkSurfaceKHR surface);
    void Cleanup();

private:
    std::vector<Imgine_CommandBuffer*> commandBuffers;
    VkCommandPool commandPool;
};

struct Imgine_CommandBufferManager : public Imgine_VulkanInstanceBind
{
    Imgine_CommandBufferManager() = delete;
    Imgine_CommandBufferManager(Imgine_Vulkan* _instance) : Imgine_VulkanInstanceBind(_instance), pool(_instance), renderFinished(_instance) {}
    Imgine_CommandBufferPool pool;
    Imgine_VulkanSemaphore renderFinished;
    void Cleanup();
};


#endif