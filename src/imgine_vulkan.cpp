#include "imgine_vulkan.h"
#include "imgine_app.h"
#include "imgine_window.h"
#include "imgine_glfw.h"
#include "imgine_vulkanhelpers.h"
#include "imgine_types.h"
#include "imgine_vulkanmemoryallocator.h"
#include <set>
#include <string>


#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

void Imgine_Vulkan::initVulkan(GLFWwindow* window)
{
    createInstance();
    setupDebugMessenger();
    createGLFWSurface(window);


    pickPhysicalDevice();
    createLogicalDevice();

    allocator = createVMAllocator(instance, device, physicalDevice);



    swapChain.createSwapChain(surface);
    swapChain.createImageViews();
    

    renderPass = renderPassManager.CreateRenderPass(&swapChain);
    

    descriptorSetLayout = layout.descriptorSetsLayout.createDescriptorTexturedSetLayout();
    
    
    pipeline.createGraphicsPipeline(&layout);
    commandBufferManager.getPool().allocateBuffers(surface);
    createDepthRessources();
    swapChain.createFrameBuffersWithDepth(&depthView);

    createTextureImage();
    
    //Shader buffer data
    createVertexBuffer();
    createIndexBuffer();
    
    //Uniform data
    uniformBuffer.Create();
    descriptorPool.createUniformTexturedPool();
    descriptorPool.allocateUBOTexturedDescriptorsSets(*descriptorSetLayout, &uniformDescriptorSets, &uniformBuffer, &view, &sampler);




    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    fences.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        commandBuffers[i] = commandBufferManager.getPool().allocateBuffers();
        fences[i] = fenceManager.getFence();
    }
    createSyncObjects();
}

void Imgine_Vulkan::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport(validationLayers)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions(enableValidationLayers);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}
bool Imgine_Vulkan::isDeviceSuitable(VkPhysicalDevice device)
{
    {
        Imgine_QueueFamilyIndices indices = findQueueFamilies(device,surface);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            Imgine_SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device,surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

}
void Imgine_Vulkan::recreateSwapChain()
{
    int width = 0, height = 0;

    Imgine_Application* app = Imgine_Application::getInstance();
    app->window.GetWindowSize(&width, &height);

    while (width == 0 || height == 0) {
        app->window.GetWindowSize(&width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    swapChain.createSwapChain(surface);
    swapChain.createImageViews();
    createDepthRessources();
    swapChain.createFrameBuffers();
}
void Imgine_Vulkan::cleanupSwapChain()
{
    vkDestroyImageView(device, depthView.view, nullptr);
    destroyImage(this, depthImage.image, depthImage.allocation);
    swapChain.CleanupSwapChain();
}

/// <summary>
/// Use a staging buffer to map and copy vertex data, and allocate device local memory vertex buffer to copy to
/// </summary>
void Imgine_Vulkan::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();
    
    VkBuffer stagingBuffer;
    VmaAllocation  stagingAllocation;
    
    createTemporaryBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, stagingAllocation);

    void* mappedData;
    copyMappedMemorytoAllocation(this, mesh.vertices.data(), stagingAllocation, static_cast<uint32_t>(mesh.vertices.size()), &mappedData);

    createBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, model.vertexBuffer, model.vertexBufferAllocation);

    copyBuffer(this, stagingBuffer, model.vertexBuffer, bufferSize);

    destroyBuffer(this, stagingBuffer, stagingAllocation);
}

void Imgine_Vulkan::createIndexBuffer() {
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VmaAllocation  stagingAllocation;
    createTemporaryBuffer(this,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, stagingAllocation);

    void* mappedData;
    copyMappedMemorytoAllocation(this, indices.data(), stagingAllocation, static_cast<uint32_t>(indices.size()), &mappedData);

    createBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, model.indexBuffer, model.indexBufferAllocation);

    copyBuffer(this, stagingBuffer, model.indexBuffer, bufferSize);
    destroyBuffer(this, stagingBuffer, stagingAllocation);
}

void Imgine_Vulkan::createTextureImage()
{
    Imgine_AssetLoader* assetLoader = Imgine_AssetLoader::GetInstance();
    imageRef = assetLoader->loadTexture(this, "textures/texture.jpg", Imgine_AssetLoader::TextureTypes::DIFFUSE);
    view.view = createImageView(this, imageRef.GetTexture().GetImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    view.imageRef = imageRef;
    createSampler(this, &sampler.sampler);
}

void Imgine_Vulkan::createDepthRessources()
{
    VkFormat depthFormat = findDepthFormat(physicalDevice);

    createImage(this,swapChain.swapChainExtent.width, swapChain.swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,  &depthImage.image, &depthImage.allocation);
    depthView.view = createImageView(this,depthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    transitionImageLayout(this,depthImage.image, depthFormat,VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void Imgine_Vulkan::createGLFWSurface(GLFWwindow* window)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}
bool Imgine_Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
}
void Imgine_Vulkan::setupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}
void Imgine_Vulkan::pickPhysicalDevice()
{
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
}
void Imgine_Vulkan::createLogicalDevice()
{
    Imgine_QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}
void Imgine_Vulkan::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    swapChain.imageAquiredSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    commandBufferManager.getSemaphores().resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        swapChain.imageAquiredSemaphores[i].setVulkanInstanceBind(this);
        commandBufferManager.getSemaphores()[i].setVulkanInstanceBind(this);
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &swapChain.imageAquiredSemaphores[i].semaphore) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &commandBufferManager.getSemaphores()[i].semaphore) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &fences[i]->fence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

void Imgine_Vulkan::draw()
{
    vkWaitForFences(device, 1, &fences[currentFrame]->fence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult res = vkAcquireNextImageKHR(device, swapChain.swapChain, UINT64_MAX, swapChain.imageAquiredSemaphores[currentFrame].semaphore, VK_NULL_HANDLE, &imageIndex);

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    else if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed recreate swap chain image !");
    }

    uniformBuffer.Update(swapChain.swapChainExtent,currentFrame);

    vkResetFences(device, 1, &fences[currentFrame]->fence);

    vkResetCommandBuffer(commandBuffers[currentFrame]->commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);

    renderPassManager.beginRenderPass(renderPass, 
        commandBuffers[currentFrame], 
        &swapChain,
        &layout,
        &uniformDescriptorSets,
        model.vertexBuffer,
        static_cast<uint32_t>(mesh.vertices.size()), 
        model.indexBuffer,
        static_cast<uint32_t>(mesh.indices.size()), 
        imageIndex,
        currentFrame);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { swapChain.imageAquiredSemaphores[currentFrame].semaphore};
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame]->commandBuffer;

    VkSemaphore signalSemaphores[] = { commandBufferManager.getSemaphores()[currentFrame].semaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fences[currentFrame]->fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    res = vkQueuePresentKHR(presentQueue, &presentInfo);


    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || Imgine_Application::getInstance()->window.framebufferResized) {
        Imgine_Application::getInstance()->window.framebufferResized = false;
        recreateSwapChain();
    }
    else if (res != VK_SUCCESS) {
        throw std::runtime_error("failed present swap chain image !");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void Imgine_Vulkan::cleanup()
{
    swapChain.Cleanup();

    sampler.Cleanup(this);
    view.Cleanup(this);

    depthView.Cleanup(this);
    depthImage.Cleanup(this);

    pipeline.cleanup(layout.pipelineLayout);
    renderPass->cleanup();
    renderPassManager.Cleanup();


    uniformBuffer.Cleanup();
    descriptorPool.cleanup(&uniformDescriptorSets);
    layout.cleanup();

    mesh.Cleanup();
    model.Cleanup(this);


    Imgine_AssetLoader::GetInstance()->Cleanup(this);


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyFence(device, fences[i]->fence, nullptr);
    }

    commandBufferManager.cleanup();


    destroyVMAllocator(allocator);
   
    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}








