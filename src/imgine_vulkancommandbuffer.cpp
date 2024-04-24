#include "imgine_vulkancommandbuffer.h"
#include <stdexcept>
#include <vector>
#include "imgine_vulkan.h"
#include "imgine_vulkanhelpers.h"

Imgine_CommandBuffer* Imgine_CommandBufferPool::allocateBuffers() {

    //Imgine_CommandBuffer* buffer = new Imgine_CommandBuffer(getVulkanInstanceBind());

    //VkCommandBufferAllocateInfo allocInfo{};
    //allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    //allocInfo.commandPool = commandPool;

    ////PRIMARY -> Allocate for queue exec, but can't be called from others cmdBuffers
    ////SECONDARY -> Can't be submitted directlt, can be called from others PRIMARY cmdBuffers
    //allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    //allocInfo.commandBufferCount = 1;


    //CHECK_VK(
    //    "failed to allocate command buffers!",
    //    vkAllocateCommandBuffers(getVulkanInstanceBind()->GetDevice(), &allocInfo, &(buffer->commandBuffer))
    //)

    //    /*if (vkAllocateCommandBuffers(getVulkanInstanceBind()->GetDevice(), &allocInfo, &(buffer->commandBuffer)) != VK_SUCCESS) {
    //        throw std::runtime_error("failed to allocate command buffers!");
    //    }*/

    Imgine_CommandBuffer* commandBuffer = new Imgine_CommandBuffer(instance, *this, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    commandBuffers.push_back(commandBuffer);
    return commandBuffer;
}


void Imgine_CommandBuffer::beginRenderPass(Imgine_VulkanRenderPass* renderPass, Imgine_SwapChain* swapChain, uint32_t imageIndex) {
    begin(VK_NO_FLAGS);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->renderPass;
    renderPassInfo.framebuffer = swapChain->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->swapChainExtent;



    std::array<VkClearValue, 2> clearValues{};
    // !! Order of clear values similar to attachment order !! 
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

}

Imgine_CommandBuffer::Imgine_CommandBuffer(Imgine_Vulkan* instance, Imgine_CommandBufferPool& pool, VkCommandBufferLevel level) :
    Imgine_VulkanInstanceBind(instance),
    commandPool(pool)
{

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.commandPool;
    allocInfo.commandBufferCount = 1;
    allocInfo.level = level;


    CHECK_VK(
        "failed to allocate command buffers!",
        vkAllocateCommandBuffers(getVulkanInstanceBind()->GetDevice(), &allocInfo, &(commandBuffer))
    )
}
Imgine_CommandBuffer::Imgine_CommandBuffer(Imgine_CommandBuffer&& other) :
    Imgine_VulkanInstanceBind(std::move(other)),
    commandBuffer(std::move(other.commandBuffer)),
    commandPool(other.commandPool),
    state(other.state)
{
    other.state = CommandBufferState::Invalid;
}
void Imgine_CommandBuffer::copyBuffertoImage(
    const Imgine_Buffer& buffer, 
    const Imgine_VulkanImage& image, 
    uint32_t width, uint32_t height)
{

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
void Imgine_CommandBuffer::copyBuffer(const Imgine_Buffer& src, const Imgine_Buffer& dst, VkDeviceSize size)
{
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src.buffer, dst.buffer, 1, &copyRegion);
}
void Imgine_CommandBuffer::imageMemoryBarrier(
    const Imgine_VulkanImage& image,
    const Imgine_ImageMemoryBarrier& barrierState)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = barrierState.old_layout;
    barrier.newLayout = barrierState.new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    if (barrierState.old_layout == VK_IMAGE_LAYOUT_UNDEFINED && barrierState.new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (barrierState.old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && barrierState.new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (barrierState.old_layout == VK_IMAGE_LAYOUT_UNDEFINED && barrierState.new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}




void Imgine_CommandBuffer::begin(VkCommandBufferUsageFlags flags) {

    assert(!is_recording() && "Command buffer is already recording, please call end before beginning again");

    state = CommandBufferState::Recording;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    CHECK_VK(
        "failed to begin recording command buffer!",
        vkBeginCommandBuffer(commandBuffer, &beginInfo) 
    )

}



void Imgine_CommandBuffer::endRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

bool Imgine_CommandBuffer::is_recording() const
{
    return state == CommandBufferState::Recording;
}

void Imgine_CommandBuffer::end() {
    assert(is_recording() && "CommandBuffer is not recording, please call begin before end");

    CHECK_VK(
        "failed to record command buffer!",
        vkEndCommandBuffer(commandBuffer)
    )

    state = CommandBufferState::Executable;

}

void Imgine_CommandBufferPool::allocateBuffers(VkSurfaceKHR surface) {

    Imgine_QueueFamilyIndices queueFamilyIndices = findQueueFamilies(getVulkanInstanceBind()->GetPhysicalDevice(), surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    CHECK_VK(
        "failed to create command pool!",
        vkCreateCommandPool(getVulkanInstanceBind()->GetDevice(), &poolInfo, nullptr, &commandPool)
    )

}


void Imgine_CommandBufferPool::cleanup() {

    for (auto b : commandBuffers) {
        delete b;
    }
    commandBuffers.clear();

    vkDestroyCommandPool(getVulkanInstanceBind()->GetDevice(), commandPool, nullptr);
}

Imgine_CommandBuffer& Imgine_CommandBufferPool::requestCommandBuffer(VkCommandBufferLevel level)
{
    Imgine_CommandBuffer* commandBuffer = new Imgine_CommandBuffer(instance, *this, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    commandBuffers.push_back(commandBuffer);
    return *commandBuffers.back();
}



VkCommandBuffer Imgine_CommandBufferManager::beginSingleTimeCommand()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool.commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(getVulkanInstanceBind()->GetDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Imgine_CommandBufferManager::endSingleTimeCommand(VkQueue queue,VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(getVulkanInstanceBind()->GetDevice(), pool.commandPool, 1, &commandBuffer);
}

void Imgine_CommandBufferManager::cleanup()
{
    pool.cleanup();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        renderFinishedSemaphores[i].Cleanup();
    }
}

Imgine_CommandBuffer& Imgine_CommandBufferManager::requestCommandBuffer()
{
    return pool.requestCommandBuffer();
}



/// <summary>
/// VMA_MEMORY_USAGE_CPU_ONLY
/// </summary>
/// <param name="instance"></param>
/// <param name="size"></param>
/// <param name="usage"></param>
/// <param name="buffer"></param>
/// <param name="allocation"></param>
void createTemporaryBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer,VmaAllocation& allocation)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; 


    vmaCreateBuffer(instance->allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    DEBUGVMAALLOC(instance->allocator, allocation, "TemporaryBuffer", "VMA_MEMORY_USAGE_CPU_ONLY", "");
}

/// <summary>
/// VMA_MEMORY_USAGE_GPU_ONLY
/// </summary>
/// <param name="instance"></param>
/// <param name="size"></param>
/// <param name="usage"></param>
/// <param name="buffer"></param>
/// <param name="allocation"></param>
void createBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


    vmaCreateBuffer(instance->allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    DEBUGVMAALLOC(instance->allocator, allocation, "Buffer", "VMA_MEMORY_USAGE_GPU_ONLY","");
}

/// <summary>
/// vmausage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
/// vmaflags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
/// </summary>
/// <param name="instance"></param>
/// <param name="size"></param>
/// <param name="usage"></param>
/// <param name="buffer"></param>
/// <param name="allocation"></param>
void createUniformBuffer(Imgine_Vulkan* instance, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    vmaCreateBuffer(instance->allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    DEBUGVMAALLOC(instance->allocator, allocation, "UniformBuffer", "VMA_MEMORY_USAGE_AUTO_PREFER_HOST", "VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT");
}

void destroyBuffer(Imgine_Vulkan* instance, VkBuffer buffer, VmaAllocation allocation) {
    DEBUGVMADESTROY(instance->allocator, allocation);
    vmaDestroyBuffer(instance->allocator, buffer, allocation);
}



/// <summary>
/// Copy src buffer to destination, submit and wait for queue to resolve immediatly
/// </summary>
/// <param name="instance"></param>
/// <param name="srcBuffer"></param>
/// <param name="dstBuffer"></param>
/// <param name="size"></param>
void copyBuffer(Imgine_Vulkan* instance,VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = instance->commandBufferManager.beginSingleTimeCommand();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    instance->commandBufferManager.endSingleTimeCommand(instance->graphicsQueue, commandBuffer);
}
