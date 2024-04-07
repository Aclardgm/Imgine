#pragma once
#ifndef ImGINE_APP
#define ImGINE_APP


#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "imgine_vulkan.h"
#include "imgine_window.h"
#include "imgine_imgui.h"


class Imgine_Application
{
public:

    Imgine_Vulkan vulkan;
    Imgine_GLFWWindow window;
    bool                        Show_demo_window = true;
    bool                        Show_another_window = false;
    ImVec4                      clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    Imgine_VulkanDescriptorPool pool;
    

    void initGUI();
    void run();
    Imgine_Application() : pool(&vulkan) { }
    Imgine_Application(const Imgine_Application&) = delete;
    Imgine_Application& operator=(const Imgine_Application&) = delete;
    Imgine_Application(Imgine_Application&&) = default;
    Imgine_Application& operator=(Imgine_Application&&) = default;
    ~Imgine_Application() = default;


    static Imgine_Application* getInstance();
    void Cleanup();
    void DrawFrame(ImDrawData* draw_data);
private:

    void MainLoop();
};


#endif