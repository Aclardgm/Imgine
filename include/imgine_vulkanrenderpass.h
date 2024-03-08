#pragma once
#ifndef ImGINE_VULKANRENDERPASS
#define ImGINE_VULKANRENDERPASS
#include <vulkan/vulkan.h>
#include <vector>
#include "imgine_vulkaninstancebind.h"

    
struct Imgine_SwapChain;
struct Imgine_CommandBuffer;
struct Imgine_VulkanDescriptorSets;
struct Imgine_VulkanLayout;

template<typename T>
concept SWAPCHAIN = std::is_base_of_v<Imgine_SwapChain, T>;


struct Imgine_VulkanRenderPass : public Imgine_VulkanInstanceBind {
public:
    //Imgine_VulkanRenderPass() {};
    Imgine_VulkanRenderPass(Imgine_Vulkan* instance, VkRenderPass pass) : Imgine_VulkanInstanceBind(instance), renderPass(pass) {}
    void cleanup();
    VkRenderPass renderPass;
};

template<typename SC>
requires SWAPCHAIN<SC>
struct Imgine_VulkanRenderPassBuilder : public Imgine_VulkanInstanceBind {

    Imgine_VulkanRenderPassBuilder() = delete;
    Imgine_VulkanRenderPassBuilder(Imgine_Vulkan* instance, SC* swapChain) : Imgine_VulkanInstanceBind(instance), swapChain(swapChain) {}
    Imgine_VulkanRenderPass* build();
    SC* swapChain = nullptr;
};



struct Imgine_VulkanRenderPassManager : public Imgine_VulkanInstanceBind {

    Imgine_VulkanRenderPassManager() = delete;
    Imgine_VulkanRenderPassManager(Imgine_Vulkan* _instance) : Imgine_VulkanInstanceBind(_instance) {}

    std::vector<Imgine_VulkanRenderPass*> renderPasses;

    void beginRenderPass(
        Imgine_VulkanRenderPass* renderPass,
        Imgine_CommandBuffer* commandBuffer,
        Imgine_SwapChain* swapChain,
        Imgine_VulkanLayout* layout,
        Imgine_VulkanDescriptorSets* descSets,
        VkBuffer vertexBuffer,
        uint32_t vertexBufferSize,
        VkBuffer indexBuffer,
        uint32_t indexBufferSize,
        uint32_t imageIndex,
        uint32_t currentFrame);
    void endRenderPass(Imgine_CommandBuffer* commandBuffer);
    Imgine_VulkanRenderPass* CreateRenderPass(Imgine_SwapChain* swapChain);


    void Cleanup();
};


#endif