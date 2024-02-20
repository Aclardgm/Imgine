#include "imgine_window.h"
#include "imgine_glfw.h"

int Imgine_GLFWWindow::SetupWindow(WindowData data)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    // Create window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWWindow = glfwCreateWindow(data.Width, data.Height, data.Title.c_str(), nullptr, nullptr);
    if (!glfwVulkanSupported())
    {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    return 0;
}

void Imgine_GLFWWindow::Cleanup()
{
    glfwDestroyWindow(GLFWWindow);
    glfwTerminate();
}
