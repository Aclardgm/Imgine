#pragma once
#ifndef ImGINE_VULKANSWAPCHAIN
#define ImGINE_VULKANSWAPCHAIN
#include <vulkan/vulkan.h>
#include <boost/optional.hpp>
#include <vector>

#include "imgine_vulkaninstancebind.h"
#include "imgine_vulkansemaphore.h"

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
    std::vector<Imgine_VulkanSemaphore> imageAquiredSemaphores;



public:
    Imgine_SwapChain() = delete;
    Imgine_SwapChain(Imgine_Vulkan* _instance) : Imgine_VulkanInstanceBind(_instance){}

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

    /// <summary>
    /// Cleanup only framebuffers
    /// </summary>
    void CleanupFrameBuffers();
    /// <summary>
    /// Cleanup all
    /// </summary>
    void Cleanup();
    /// <summary>
    /// Cleanup FrameBuffers/ImageViews/SwapChainKHR only
    /// </summary>
    void CleanupSwapChain();

};


#endif