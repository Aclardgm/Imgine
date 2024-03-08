#pragma once
#ifndef ImGINE_APP
#define ImGINE_APP


#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "imgine_vulkan.h"
#include "imgine_window.h"


class Imgine_Application
{
public:

    Imgine_Vulkan vulkan;
    Imgine_GLFWWindow window;
    
    void run();
    Imgine_Application() { }
    Imgine_Application(const Imgine_Application&) = delete;
    Imgine_Application& operator=(const Imgine_Application&) = delete;
    Imgine_Application(Imgine_Application&&) = default;
    Imgine_Application& operator=(Imgine_Application&&) = default;
    ~Imgine_Application() = default;


    static Imgine_Application* getInstance();
    void Cleanup();
    void DrawFrame();
private:

    void MainLoop();
};


#endif