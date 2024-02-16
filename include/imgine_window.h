#pragma once
#ifndef ImGINE_WINDOW
#define ImGINE_WINDOW

#include "imgine_glfw.h"


class ImgineWindow
{
public :
    virtual int Setup() = 0;
};


class ImgineGLFWWindow : public ImgineWindow
{
	GLFWwindow* Window;
    
public:
    virtual int Setup() override 
	{
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;
        // Create window with Vulkan context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);
        if (!glfwVulkanSupported())
        {
            printf("GLFW: Vulkan Not Supported\n");
            return 1;
        }
        return 0;
	}


};







#endif