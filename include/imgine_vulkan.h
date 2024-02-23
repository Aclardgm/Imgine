#pragma once
#ifndef ImGINE_VULKAN
#define ImGINE_VULKAN


#include <vulkan/vulkan.h>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkanswapchain.h"
#include "imgine_vulkanrenderpass.h"
#include "imgine_vulkancommandbuffer.h"
#include "imgine_vulkanfence.h"
#include "imgine_vulkanimage.h"
#include "imgine_vulkanpipeline.h"


struct GLFWwindow;

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

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};


/// Helpers
static inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
static inline void check_vk_result(VkResult err);
inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
inline bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
VkFormat findSupportedFormat(VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
VkFormat findDepthFormat(VkPhysicalDevice& physicalDevice);
uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);


#endif