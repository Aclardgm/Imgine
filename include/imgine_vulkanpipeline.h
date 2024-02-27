#pragma once
#ifndef ImGINE_VULKANPIPELINE
#define ImGINE_VULKANPIPELINE
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkandescriptorsets.h"
#include "imgine_glm.h"

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    /// <summary>
    /// Describe vertex stride and vertex/instance rate rendering
    /// </summary>
    /// <returns></returns>
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    /// <summary>
    /// Format of each params of vertex
    /// </summary>
    /// <returns></returns>
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};



static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

struct Imgine_VulkanPipeline : public Imgine_VulkanInstanceBind {
    //VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
public:
    Imgine_VulkanPipeline() = delete;
    Imgine_VulkanPipeline(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {
    }

    void Cleanup(VkPipelineLayout layout);
    void createGraphicsPipeline(Imgine_VulkanLayout* layout);
    VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif