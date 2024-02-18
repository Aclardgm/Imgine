#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>


#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort


#include <iostream>

#include "imgine_jobsystem.h"

// Main code
int main(int, char**)
{
   /* CheckGLM();
    CheckFlecs();

    AssimpImportScene("faketarget");
    STBImport("faketarget");*/


    //MainFunction fct = [](void) -> int 
    //{
    //    //std::cout << "Application Running" << std::endl;
    //    return 0;
    //};


    //Imgine_Application application;

    //application.run(fct);


    imgine::jobsystem::test();



    return 0;
}