#pragma once
#ifndef ImGINE_WINDOW
#define ImGINE_WINDOW

#include "imgine_glfw.h"


struct WindowData
{
    uint32_t Width;
    uint32_t Height;
    std::string Title;
};


struct Imgine_GLFWWindow 
{
    bool framebufferResized = false;
public:
	GLFWwindow* GLFWWindow;
    int SetupWindow(WindowData data);
    void Cleanup();
    GLFWwindow* getWindow() { return GLFWWindow; };
    static void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Imgine_GLFWWindow*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }
    void GetWindowSize(int* width, int* height);

};







#endif