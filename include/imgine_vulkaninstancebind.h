#pragma once
#ifndef ImGINE_VULKANINSTANCEBIND
#define ImGINE_VULKANINSTANCEBIND
#include <assert.h>

struct Imgine_Vulkan;

struct Imgine_VulkanInstanceBind {
public:
    Imgine_VulkanInstanceBind(Imgine_Vulkan* _instance) : instance(_instance) {}

    inline Imgine_Vulkan* getVulkanInstanceBind() const {
        assert(instance != nullptr);
        return instance;
    }

    inline void setVulkanInstanceBind(Imgine_Vulkan* _instance) {
        instance = _instance;
    }

private:
    Imgine_Vulkan* instance;
};



#endif