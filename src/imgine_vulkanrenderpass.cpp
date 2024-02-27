#include "imgine_vulkanrenderpass.h"

#include "imgine_vulkan.h"
#include "imgine_vulkancommandbuffer.h"
#include "imgine_vulkanswapchain.h"
#include "imgine_vulkandescriptorsets.h"
#include "imgine_vulkanressources.h"
#include "imgine_vulkanpipeline.h"

Imgine_VulkanRenderPass* Imgine_VulkanRenderPassManager::CreateRenderPass(Imgine_SwapChain* swapChain) {

    Imgine_VulkanRenderPass* renderpass = Imgine_VulkanRenderPassBuilder<Imgine_SwapChain>(GetVulkanInstanceBind(), swapChain).Build();

    renderPasses.push_back(renderpass);

    return renderpass;
}

void Imgine_VulkanRenderPassManager::Cleanup()
{
    for (Imgine_VulkanRenderPass* rd : renderPasses) {
        delete rd;
    }
}



template<typename T>
requires SWAPCHAIN<T>
Imgine_VulkanRenderPass* Imgine_VulkanRenderPassBuilder<T>::Build()
{
    VkRenderPass renderPass = { 0 };

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChain->swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;


    if (vkCreateRenderPass(GetVulkanInstanceBind()->GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return new Imgine_VulkanRenderPass(GetVulkanInstanceBind(), std::move(renderPass));
}


void Imgine_VulkanRenderPass::Cleanup()
{
    vkDestroyRenderPass(GetVulkanInstanceBind()->GetDevice(), renderPass, nullptr);
}


void Imgine_VulkanRenderPassManager::BeginRenderPass(
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
    uint32_t currentFrame) {

    VkCommandBuffer cmdBuffer = commandBuffer->commandBuffer;

    commandBuffer->BeginRenderPass(renderPass, swapChain, imageIndex);

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetVulkanInstanceBind()->pipeline.graphicsPipeline);
    
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain->swapChainExtent.width;
    viewport.height = (float)swapChain->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain->swapChainExtent;
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout->PipelineLayout, 0, 1, &descSets->descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(cmdBuffer, indexBufferSize, 1, 0, 0, 0);
    
    vkCmdEndRenderPass(cmdBuffer);

    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
void Imgine_VulkanRenderPassManager::EndRenderPass(Imgine_CommandBuffer* commandBuffer) {
    commandBuffer->EndRenderPass();
    commandBuffer->End();
}
