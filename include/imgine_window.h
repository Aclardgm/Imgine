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
public:
	GLFWwindow* GLFWWindow;
    int SetupWindow(WindowData data);

    void Cleanup();
    GLFWwindow* getWindow() { return GLFWWindow; };
};







#endif