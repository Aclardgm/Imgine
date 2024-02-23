#pragma once
#ifndef ImGINE_VULKANINSTANCEBIND
#define ImGINE_VULKANINSTANCEBIND
#include <assert.h>

struct Imgine_Vulkan;

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



#endif