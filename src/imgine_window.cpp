#include "imgine_window.h"
#include "imgine_glfw.h"

int Imgine_GLFWWindow::SetupWindow(WindowData data)
{

    int errr = glfwInit();
    std::cout << errr << "\n";

    // Create window with Vulkan context
    glfwSetErrorCallback(glfw_error_callback);
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
