#include "imgine_app.h"
#include "imgine_window.h"
#include "imgine_vulkan.h"
#include "imgine_vulkanhelpers.h"

void Imgine_Application::initGUI()
{
    //Setup descriptor pool for ImGui
    pool.createTexturedPool();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window.getWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkan.instance;
    init_info.PhysicalDevice = vulkan.physicalDevice;
    init_info.Device = vulkan.device;
    init_info.QueueFamily = findQueueFamilies(vulkan.physicalDevice,vulkan.surface).graphicsFamily.value();
    init_info.Queue = vulkan.graphicsQueue;
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = pool.descriptorPool;
    init_info.RenderPass = vulkan.renderPass->renderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = vulkan.swapChain.GetImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info);
}

void Imgine_Application::run()
{
    WindowData data = {
        1280,720,"Dear ImGui GLFW + Vulkan example"
    };

    window.SetupWindow(data);
    vulkan.initVulkan(window.GLFWWindow);
    initGUI();
    MainLoop();
}

Imgine_Application* Imgine_Application::getInstance()
{
    static Imgine_Application instance;
    return &instance;
}

void Imgine_Application::MainLoop()
{
    ImGuiIO& io = ImGui::GetIO();
    while (!glfwWindowShouldClose(window.GLFWWindow)) {
        glfwPollEvents();
        
        
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo Window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (Show_demo_window)
            ImGui::ShowDemoWindow(&Show_demo_window);

        // 2. Show a simple Window that we create ourselves. We use a Begin/End pair to create a named Window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a Window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &Show_demo_window);      // Edit bools storing our Window open/close state
            ImGui::Checkbox("Another Window", &Show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple Window.
        if (Show_another_window)
        {
            ImGui::Begin("Another Window", &Show_another_window);   // Pass a pointer to our bool variable (the Window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another Window!");
            if (ImGui::Button("Close Me"))
                Show_another_window = false;
            ImGui::End();
        }
        
        // Rendering
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!is_minimized)
        {
            DrawFrame(draw_data);
        }

    }
    vkDeviceWaitIdle(vulkan.device);
}


void Imgine_Application::DrawFrame(ImDrawData* draw_data)
{
    uint32_t imageIndex = vulkan.draw();

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, vulkan.commandBuffers[vulkan.currentFrame]->commandBuffer);
    vulkan.enddraw();
    vulkan.present(imageIndex);

}
void Imgine_Application::Cleanup()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    pool.cleanup(&vulkan.uniformDescriptorSets);

    vulkan.cleanup();
    window.Cleanup();
};