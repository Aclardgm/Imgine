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

    void cleanup(VkPipelineLayout layout);
    void createGraphicsPipeline(Imgine_VulkanLayout* layout);
    VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif