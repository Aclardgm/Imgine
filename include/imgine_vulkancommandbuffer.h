#pragma once
#ifndef ImGINE_VULKANCOMMANDBUFFER
#define ImGINE_VULKANCOMMANDBUFFER
#include <vulkan/vulkan.h>
#include <vector>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkansemaphore.h"
#include "imgine_vulkanmemoryallocator.h"

struct Imgine_VulkanRenderPass;
struct Imgine_SwapChain;
struct Imgine_CommandBufferManager;

/// VMA helpers
void copyBuffer(Imgine_Vulkan* instance, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);
void createUniformBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);
void createTemporaryBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);
void destroyBuffer(Imgine_Vulkan* instance, VkBuffer buffer, VmaAllocation allocation);
void destroyImage(Imgine_Vulkan* instance, VkImage image, VmaAllocation allocation);


struct Imgine_CommandBuffer : public Imgine_VulkanInstanceBind {


public:
    Imgine_CommandBuffer(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}

    /// <summary>
    /// vkBeginCommandBuffer
    /// </summary>
    void begin();
    void end();
    void beginRenderPass(Imgine_VulkanRenderPass* renderPass, Imgine_SwapChain* swapChain, uint32_t imageIndex);
    void endRenderPass();
    VkCommandBuffer commandBuffer;
};



struct Imgine_CommandBufferPool : public Imgine_VulkanInstanceBind {
    friend Imgine_CommandBufferManager;
public:
    Imgine_CommandBufferPool() = delete;
    Imgine_CommandBufferPool(Imgine_Vulkan* instance) :Imgine_VulkanInstanceBind(instance) {}



    Imgine_CommandBuffer* allocateBuffers();
    void allocateBuffers(VkSurfaceKHR surface);
    void cleanup();

private:
    std::vector<Imgine_CommandBuffer*> commandBuffers;
    VkCommandPool commandPool;
};

struct Imgine_CommandBufferManager : public Imgine_VulkanInstanceBind
{
    Imgine_CommandBufferManager() = delete;
    Imgine_CommandBufferManager(Imgine_Vulkan* _instance) : Imgine_VulkanInstanceBind(_instance), pool(_instance) {}
    
    VkCommandPool getCommandPool() { return pool.commandPool; }
    Imgine_CommandBufferPool& getPool() { return pool; }
    std::vector<Imgine_VulkanSemaphore>& getSemaphores() { return renderFinishedSemaphores; }
    
    VkCommandBuffer beginSingleTimeCommand();
    void endSingleTimeCommand(VkQueue queue, VkCommandBuffer commandBuffer);
    
    void cleanup();
private:
    Imgine_CommandBufferPool pool;
    std::vector<Imgine_VulkanSemaphore> renderFinishedSemaphores;
};


#endif
