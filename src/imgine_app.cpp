#include "imgine_app.h"
#include "imgine_window.h"
#include "imgine_vulkan.h"

void Imgine_Application::run()
{
    WindowData data = {
        1280,720,"Dear ImGui GLFW + Vulkan example"
    };

    window.SetupWindow(data);
    vulkan.initVulkan(window.GLFWWindow);
    MainLoop();
}

Imgine_Application* Imgine_Application::getInstance()
{
    static Imgine_Application instance;
    return &instance;
}

void Imgine_Application::MainLoop()
{
    while (!glfwWindowShouldClose(window.GLFWWindow)) {
        glfwPollEvents();
        DrawFrame();
    }
    vkDeviceWaitIdle(vulkan.device);
}


void Imgine_Application::DrawFrame()
{
    vulkan.draw();

}
void Imgine_Application::Cleanup()
{
    vulkan.cleanup();
    window.Cleanup();
};