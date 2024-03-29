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
#include "imgine_vulkandescriptorsets.h"
#include "imgine_vulkanressources.h"
#include "imgine_assetloader.h"
#include "imgine_vulkanmemoryallocator.h"

const int MAX_FRAMES_IN_FLIGHT = 2;
struct GLFWwindow;

struct Imgine_Vulkan
{
public:
    Imgine_Vulkan() :
        layout(this),
        swapChain(this),
        pipeline(this),
        renderPassManager(this),
        commandBufferManager(this),
        fenceManager(this),
        uniformBuffer(this),
        uniformDescriptorSets(this),
        descriptorPool(this) {}
    void initVulkan(GLFWwindow* window);
    void cleanup();
    void draw();

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

    //Pipeline & Descriptor Layouts
    Imgine_VulkanLayout layout;

    //UBO
    Imgine_VulkanUniformBuffer uniformBuffer;
    Imgine_VulkanDescriptorSets uniformDescriptorSets;
    Imgine_VulkanDescriptorPool descriptorPool;
    VkDescriptorSetLayout* descriptorSetLayout;


    //Command Buffers
    Imgine_CommandBufferManager commandBufferManager;
    std::vector<Imgine_CommandBuffer*> commandBuffers;


    //Fence Manager
    Imgine_VulkanFenceManager fenceManager;
    std::vector<Imgine_VulkanFence*> fences;

    uint32_t currentFrame = 0;


    Imgine_Mesh mesh = Imgine_Mesh({
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        }, 
        {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        }
    );
    Imgine_VulkanModel model;

    //Image data
    Imgine_TextureRef imageRef;
    Imgine_VulkanImageView view;
    Imgine_VulkanImageSampler sampler;

    //Depth data
    Imgine_VulkanImage depthImage;
    Imgine_VulkanImageView depthView;


    VmaAllocator allocator;

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
    void createVertexBuffer();
    void createIndexBuffer();
    void createTextureImage();
    void createDepthRessources();
    

    /// <summary>
    /// TODO : create a new swapChain, store current in oldSwapChain of VkSwapchainCreateInfoKHR, delete old as soon as useless
    /// to keep drawing with old while starting drawing with new
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    void recreateSwapChain();
    void cleanupSwapChain();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};



#endif