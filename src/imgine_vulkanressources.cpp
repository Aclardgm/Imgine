#include "imgine_vulkanressources.h"
#include "imgine_vulkanpipeline.h"
#include "imgine_vulkan.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include "imgine_vulkanmemoryallocator.h"

void Imgine_VulkanUniformBuffer::Create()
{
    Imgine_Vulkan* vk = GetVulkanInstanceBind();

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersAllocation.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createUniformBuffer(vk,bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,uniformBuffers[i], uniformBuffersAllocation[i]);
        vmaMapMemory(vk->allocator, uniformBuffersAllocation[i], &(uniformBuffersMapped[i]));
    }
}

void Imgine_VulkanUniformBuffer::Cleanup()
{
    Imgine_Vulkan* vk = GetVulkanInstanceBind();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vmaUnmapMemory(vk->allocator, uniformBuffersAllocation[i]);
        destroyBuffer(vk, uniformBuffers[i], uniformBuffersAllocation[i]);
    }
}

void Imgine_VulkanUniformBuffer::Update(VkExtent2D extent,uint32_t imageIndex)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[imageIndex], &ubo, sizeof(ubo));
}
