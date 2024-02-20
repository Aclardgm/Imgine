#pragma once


#ifndef ImGINE_GLFW
#define ImGINE_GLFW

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>


static inline void glfw_error_callback(int error, const char* description)
{
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

}

std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

#endif