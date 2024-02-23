#include "imgine_vulkan.h"
#include "imgine_vulkansemaphore.h"

void Imgine_VulkanSemaphore::Cleanup()
{
    vkDestroySemaphore(GetVulkanInstanceBind()->GetDevice(), semaphore, nullptr);
}
