#include "imgine_vulkan.h"
#include "imgine_vulkansemaphore.h"

void Imgine_VulkanSemaphore::Cleanup()
{
    vkDestroySemaphore(getVulkanInstanceBind()->GetDevice(), semaphore, nullptr);
}
