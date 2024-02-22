#pragma once
#ifndef ImGINE_VULKAN
#define ImGINE_VULKAN
#include <vulkan/vulkan.h>
#include <boost/optional.hpp>
#include <set>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>



struct GLFWwindow;
struct Imgine_VulkanImage;
struct Imgine_VulkanImageView;
struct Imgine_VulkanInstanceBind;
struct Imgine_VulkanPipeline;
struct Imgine_SwapChainSupportDetails;
struct Imgine_QueueFamilyIndices;
struct Imgine_SwapChain;
template<typename T>
concept SWAPCHAIN = std::is_base_of_v<Imgine_SwapChain, T>;
struct Imgine_VulkanRenderPass;
template<typename SC>
requires SWAPCHAIN<SC>
struct Imgine_VulkanRenderPassBuilder;
struct Imgine_Vulkan;
struct Imgine_CommandBuffer;
struct Imgine_VulkanRenderPassManager;



static std::vector<char> readFile(const std::string& filename);
static inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

static inline void check_vk_result(VkResult err);


inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

inline bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);


VkFormat findSupportedFormat(VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
VkFormat findDepthFormat(VkPhysicalDevice& physicalDevice);

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);



struct Imgine_VulkanImage {

    VkImage image;
    VkDeviceMemory imageMemory;
};

struct Imgine_VulkanImageView {
    Imgine_VulkanImage* image;
    VkImageView view;
};


struct Imgine_VulkanInstanceBind {
public:
    Imgine_VulkanInstanceBind(Imgine_Vulkan* _instance = nullptr) : instance(_instance) {}

    inline Imgine_Vulkan* GetVulkanInstanceBind() const {
        assert(instance != nullptr);
        return instance;
    }

    inline void SetVulkanInstanceBind(Imgine_Vulkan* _instance) {
        instance = _instance;
    }

private:
    Imgine_Vulkan* instance;
};



struct Imgine_VulkanFence : public Imgine_VulkanInstanceBind {
    Imgine_VulkanFence() = delete;
    Imgine_VulkanFence(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance), fence{0} {}

    VkFence fence;
};



struct Imgine_VulkanFenceManager : public Imgine_VulkanInstanceBind {
    Imgine_VulkanFenceManager(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {}

    /// <summary>
    /// Check if fences are available in freeFences, return one if so, or build new one and push it to used
    /// </summary>
    /// <returns></returns>
    Imgine_VulkanFence* GetFence();
    /// <summary>
    /// Reset given fence
    /// </summary>
    /// <param name="fence"></param>
    void ResetFence(Imgine_VulkanFence* fence);

    /// <summary>
    /// Set given fence to nullptr
    /// </summary>
    /// <param name="fence"></param>
    void ReleaseFence(Imgine_VulkanFence*& fence);

private:
    std::vector<Imgine_VulkanFence*> freeFences;
    std::vector<Imgine_VulkanFence*> usedFences;
};



struct Imgine_VulkanPipeline : public Imgine_VulkanInstanceBind {
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
public:
    Imgine_VulkanPipeline() = delete;
    Imgine_VulkanPipeline(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance) {
    }

    void Cleanup();

    void createGraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);


};

struct Imgine_VulkanSemaphore : public Imgine_VulkanInstanceBind{

    Imgine_VulkanSemaphore() = delete;
    Imgine_VulkanSemaphore(Imgine_Vulkan* instance) : Imgine_VulkanInstanceBind(instance), semaphore{ 0 } {}
    void Cleanup();
    VkSemaphore semaphore;
};



/// <summary>
/// Describe swapChain Capabilities, Format and Present mode
/// </summary>
struct Imgine_SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Imgine_QueueFamilyIndices {
    boost::optional<uint32_t> graphicsFamily;
    boost::optional<uint32_t> presentFamily;

    bool isComplete();
};


Imgine_SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
Imgine_QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);


struct Imgine_SwapChain : public Imgine_VulkanInstanceBind {
    //SwapChain
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

  /*  Imgine_VulkanImage colorImage;
    Imgine_VulkanImage depthImage;
    Imgine_VulkanImageView colorImageView;
    Imgine_VulkanImageView depthImageView;*/

    Imgine_VulkanSemaphore imageAcquired;

public:
    Imgine_SwapChain() = delete;
    Imgine_SwapChain(Imgine_Vulkan * _instance) : Imgine_VulkanInstanceBind(_instance), imageAcquired(_instance) {}

    VkFormat GetFormat() { return swapChainImageFormat; }


    void createSwapChain(VkSurfaceKHR surface);

    void createFrameBuffers();
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
        VkSampleCountFlagBits numSamples,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CleanupFrameBuffers();
    void CleanupSwapChain();

};

struct Imgine_VulkanRenderPass : public Imgine_VulkanInstanceBind {
public:
    //Imgine_VulkanRenderPass() {};
    Imgine_VulkanRenderPass(Imgine_Vulkan* instance, VkRenderPass&& pass) : Imgine_VulkanInstanceBind(instance),renderPass(std::move(pass)) {}
    void Cleanup();


    VkRenderPass renderPass;
};




template<typename SC>
requires SWAPCHAIN<SC>
struct Imgine_VulkanRenderPassBuilder : public Imgine_VulkanInstanceBind {

    Imgine_VulkanRenderPassBuilder() = delete;
    Imgine_VulkanRenderPassBuilder(Imgine_Vulkan* instance, SC* swapChain) : Imgine_VulkanInstanceBind(instance), swapChain(swapChain) {}
    Imgine_VulkanRenderPass* Build();
    SC* swapChain = nullptr;
};



struct Imgine_VulkanRenderPassManager : public Imgine_VulkanInstanceBind {

    Imgine_VulkanRenderPassManager() = delete;
    Imgine_VulkanRenderPassManager(Imgine_Vulkan* _instance) : Imgine_VulkanInstanceBind(_instance) {}

    std::vector<Imgine_VulkanRenderPass*> renderPasses;

    void BeginRenderPass(
        Imgine_VulkanRenderPass* renderPass,
        Imgine_CommandBuffer* commandBuffer,
        Imgine_SwapChain* swapChain,
        uint32_t imageIndex);
    void EndRenderPass(Imgine_CommandBuffer* commandBuffer);
    Imgine_VulkanRenderPass* CreateRenderPass(Imgine_SwapChain* swapChain);
};



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
    Imgine_CommandBufferManager(Imgine_Vulkan* _instance) : Imgine_VulkanInstanceBind(_instance),pool(_instance), renderFinished(_instance){}
    Imgine_CommandBufferPool pool;
    Imgine_VulkanSemaphore renderFinished;
    void Cleanup();

};

struct Imgine_Vulkan
{
public:
    Imgine_Vulkan() : swapChain(this), pipeline(this), renderPassManager(this), commandBufferManager(this), fenceManager(this) {}
    void initVulkan(GLFWwindow* window);
    void Cleanup();
    void Draw();
    const VkPhysicalDevice GetPhysicalDevice() const {
        return physicalDevice;
    }
    const VkDevice GetDevice() const {
        return device;
    }



    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    //Devices
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    
    //Queues
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;

    //SwapChain
    Imgine_SwapChain swapChain;
    Imgine_VulkanPipeline pipeline;
    Imgine_VulkanRenderPass* renderPass = nullptr;
    Imgine_VulkanRenderPassManager renderPassManager;


    //Command Buffer/Pool
    Imgine_CommandBufferManager commandBufferManager;
    Imgine_CommandBuffer* commandBuffer = nullptr;
    //Fence Manager
    Imgine_VulkanFenceManager fenceManager;
    Imgine_VulkanFence* fence = nullptr;


private:

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };



    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();


    
    
    void createInstance();
    void createGLFWSurface(GLFWwindow* window);
    
    void createSyncObjects();



    //void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    //    VkCommandBufferBeginInfo beginInfo{};
    //    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    //        throw std::runtime_error("failed to begin recording command buffer!");
    //    }

    //    VkRenderPassBeginInfo renderPassInfo{};
    //    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //    renderPassInfo.renderPass = renderPass.renderPass;
    //    renderPassInfo.framebuffer = swapChain.swapChainFramebuffers[imageIndex];
    //    renderPassInfo.renderArea.offset = { 0, 0 };
    //    renderPassInfo.renderArea.extent = swapChain.swapChainExtent;

    //    std::array<VkClearValue, 2> clearValues{};
    //    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    //    clearValues[1].depthStencil = { 1.0f, 0 };

    //    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    //    renderPassInfo.pClearValues = clearValues.data();

    //    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline. graphicsPipeline);

    //    VkViewport viewport{};
    //    viewport.x = 0.0f;
    //    viewport.y = 0.0f;
    //    viewport.width = (float)swapChain.swapChainExtent.width;
    //    viewport.height = (float)swapChain.swapChainExtent.height;
    //    viewport.minDepth = 0.0f;
    //    viewport.maxDepth = 1.0f;
    //    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    //    VkRect2D scissor{};
    //    scissor.offset = { 0, 0 };
    //    scissor.extent = swapChain.swapChainExtent;
    //    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    //    VkBuffer vertexBuffers[] = { vertexBuffer };
    //    VkDeviceSize offsets[] = { 0 };
    //    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    //    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    //    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

    //    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    //    vkCmdEndRenderPass(commandBuffer);

    //    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    //        throw std::runtime_error("failed to record command buffer!");
    //    }
    //}

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};




#endif