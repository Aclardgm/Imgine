#pragma once
#ifndef ImGINE_VULKANCOMMANDBUFFER
#define ImGINE_VULKANCOMMANDBUFFER
#include <vulkan/vulkan.h>
#include <vector>

#include "imgine_vulkancommons.h"
#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkansemaphore.h"
#include "imgine_vulkanmemoryallocator.h"
#include "imgine_vulkanhelpers.h"

struct Imgine_VulkanRenderPass;
struct Imgine_SwapChain;
struct Imgine_CommandBufferManager;
struct Imgine_CommandBufferPool;
struct Imgine_Buffer;
struct Imgine_VulkanImage;
struct Imgine_VulkanImageView;
/// VMA helpers
void copyBuffer(Imgine_Vulkan* instance, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);
void createUniformBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);
void createTemporaryBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);
void destroyBuffer(Imgine_Vulkan* instance, VkBuffer buffer, VmaAllocation allocation);
void destroyImage(Imgine_Vulkan* instance, VkImage image, VmaAllocation allocation);

/// <summary>
/// Do not allow copy constructor
/// </summary>
struct Imgine_CommandBuffer : public Imgine_VulkanInstanceBind {


public:
    enum class CommandBufferState
    {
        Invalid,
        Initial,
        Recording,
        Executable,
    };

    Imgine_CommandBuffer(Imgine_Vulkan* instance, Imgine_CommandBufferPool& pool, VkCommandBufferLevel level);
    Imgine_CommandBuffer(const Imgine_CommandBuffer&) = delete;
    Imgine_CommandBuffer(Imgine_CommandBuffer&& other);

    void copyBuffertoImage(const Imgine_Buffer& buffer, const Imgine_VulkanImage& image,uint32_t width,uint32_t height);
    void copyBuffer(const Imgine_Buffer& src, const Imgine_Buffer& dst, VkDeviceSize size);
    void imageMemoryBarrier(const Imgine_VulkanImage& image, const Imgine_ImageMemoryBarrier& barrierState);

    /// <summary>
    /// vkBeginCommandBuffer
    /// </summary>
    void begin(VkCommandBufferUsageFlags flags);
    void end();
    void beginRenderPass(Imgine_VulkanRenderPass* renderPass, Imgine_SwapChain* swapChain, uint32_t imageIndex);
    void endRenderPass();
    bool is_recording() const;


    CommandBufferState state{ CommandBufferState::Initial };
    Imgine_CommandBufferPool& commandPool;
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

    Imgine_CommandBuffer& requestCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);


    VkCommandPool commandPool;
private:
    std::vector<Imgine_CommandBuffer*> commandBuffers;
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
    
    Imgine_CommandBuffer& requestCommandBuffer();



    void cleanup();
private:
    Imgine_CommandBufferPool pool;
    std::vector<Imgine_VulkanSemaphore> renderFinishedSemaphores;
};


#endif
