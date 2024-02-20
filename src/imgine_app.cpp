#include "imgine_app.h"
#include "imgine_window.h"
#include "imgine_vulkan.h"

void Imgine_Application::run()
{
    WindowData data = {
        1280,720,"Dear ImGui GLFW + Vulkan example"
    };

    Window.SetupWindow(data);
    Vulkan.initVulkan(Window.GLFWWindow);
    MainLoop();
    Cleanup();
}

Imgine_Application* Imgine_Application::getInstance()
{
    static Imgine_Application instance;
    return &instance;
}

void Imgine_Application::MainLoop()
{
    while (!glfwWindowShouldClose(Window.GLFWWindow)) {
        glfwPollEvents();

    }
}
void Imgine_Application::Cleanup()
{
    Vulkan.Cleanup();
    Window.Cleanup();
};