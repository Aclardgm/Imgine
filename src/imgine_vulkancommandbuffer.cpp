#include "imgine_vulkancommandbuffer.h"
#include <stdexcept>
#include <vector>
#include "imgine_vulkan.h"

Imgine_CommandBuffer* Imgine_CommandBufferPool::Create() {

    Imgine_CommandBuffer* buffer = new Imgine_CommandBuffer();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;

    //PRIMARY -> Allocate for queue exec, but can't be called from others cmdBuffers
    //SECONDARY -> Can't be submitted directlt, can be called from others PRIMARY cmdBuffers
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(GetVulkanInstanceBind()->GetDevice(), &allocInfo, &(buffer->commandBuffer)) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    commandBuffers.push_back(buffer);
    return buffer;
}


void Imgine_CommandBuffer::BeginRenderPass(Imgine_VulkanRenderPass* renderPass, Imgine_SwapChain* swapChain, uint32_t imageIndex) {
    Begin();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->renderPass;
    renderPassInfo.framebuffer = swapChain->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

}
void Imgine_CommandBuffer::Begin() {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}



void Imgine_CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void Imgine_CommandBuffer::End() {
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Imgine_CommandBufferPool::Create(VkSurfaceKHR surface) {

    Imgine_QueueFamilyIndices queueFamilyIndices = findQueueFamilies(GetVulkanInstanceBind()->GetPhysicalDevice(), surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(GetVulkanInstanceBind()->GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}


void Imgine_CommandBufferPool::Cleanup() {

    for (auto b : commandBuffers) {
        delete b;
    }
    commandBuffers.clear();

    vkDestroyCommandPool(GetVulkanInstanceBind()->GetDevice(), commandPool, nullptr);
}



void Imgine_CommandBufferManager::Cleanup()
{
    pool.Cleanup();
    renderFinished.Cleanup();
}