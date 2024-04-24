#pragma once
#ifndef ImGINE_VULKANINSTANCEBIND
#define ImGINE_VULKANINSTANCEBIND
#include <assert.h>

struct Imgine_Vulkan;

struct Imgine_VulkanInstanceBind {
public:
    Imgine_VulkanInstanceBind(Imgine_Vulkan* _instance = nullptr) : instance(_instance) {}

    inline Imgine_Vulkan* getVulkanInstanceBind() const {
        assert(instance != nullptr &&& "Using invalid VulkanInstanceBind, do not use previously moved data or copy invalid data !");
        return instance;
    }

    inline void setVulkanInstanceBind(Imgine_Vulkan* _instance) {
        instance = _instance;
    }

protected:
    Imgine_Vulkan* instance;
};



#endif