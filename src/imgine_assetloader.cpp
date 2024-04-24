#include "imgine_assetloader.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include "imgine_vulkancommandbuffer.h"
#include "imgine_vulkan.h"
#include "imgine_vulkanressources.h"
#include "imgine_vulkanimage.h"
#include "imgine_assimp.h"
#include "imgine_types.h"
#include "imgine_mesh.h"
#include "imgine_vulkancommons.h"


void loadImage(Imgine_Vulkan* instance,std::string path, Imgine_VulkanImage& image)
{
    VkDevice device = instance->GetDevice();
    int width, height, colorDefinition;

    stbi_uc* pixels = stb_Import(path, width, height, colorDefinition);

    VkDeviceSize imageSize = width * height * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    Imgine_CommandBuffer& commandBuffer = instance->commandBufferManager.requestCommandBuffer();

    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);


    Imgine_Buffer stagingBuffer{ instance,imageSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT ,VMA_MEMORY_USAGE_CPU_ONLY };
    stagingBuffer.update(pixels, imageSize, 0);
    

    createImage(
        instance,
        width,
        height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &image.image,
        &image.allocation);

    Imgine_ImageMemoryBarrier barrier;


    {
        barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.src_access_mask = 0;
        barrier.dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.src_stage_mask = VK_PIPELINE_STAGE_HOST_BIT;
        barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        commandBuffer.imageMemoryBarrier(image, barrier);
    }

    commandBuffer.copyBuffertoImage(stagingBuffer, image, width, height);


    {
        barrier.old_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.src_access_mask = 0;
        barrier.dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.src_stage_mask = VK_PIPELINE_STAGE_HOST_BIT;
        barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        commandBuffer.imageMemoryBarrier(image, barrier);
    }


    //instance->commandBufferManager.endSingleTimeCommand(instance->graphicsQueue, commandBuffer.commandBuffer);

    commandBuffer.end();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer.commandBuffer;

    vkQueueSubmit(instance->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(instance->graphicsQueue);

    vkFreeCommandBuffers(instance->GetDevice(), commandBuffer.commandPool.commandPool, 1, &commandBuffer.commandBuffer);

    stagingBuffer.Cleanup();
    
    //createTextureImage(instance, imageSize,pixels,width,height, image, allocation);

    stbi_image_free(pixels);
}

std::vector<Imgine_Mesh> loadMeshes(Imgine_Vulkan* instance, std::string path,unsigned int flags)
{
    std::vector<Imgine_Mesh> meshes;
    AssimpImportScene(instance,path, meshes,flags);
    return meshes;
}

//Check if texture already loaded, return no value optional if not loaded
std::optional<Imgine_TextureRef> Imgine_AssetLoader::GetTextureRef(const char* path)
{
    for (unsigned int j = 0; j < loadedTextures.size(); j++)
    {
        if (std::strcmp(loadedTextures[j].path.data(), path) == 0)
        {
            Imgine_TextureRef ref = { j };
            return std::optional<Imgine_TextureRef>(ref);
        }
    }

    return std::optional<Imgine_TextureRef>();
}


//Check if texture already exists, load it if not, return corresponding assetRef 
Imgine_TextureRef Imgine_AssetLoader::loadTexture(Imgine_Vulkan* instance, const char* path,std::string typeName = "texture_diffuse")
{
    std::optional<Imgine_TextureRef> ref = GetTextureRef(path);
    if (!ref.has_value())
    {   // if texture hasn't been loaded already, load it
        Imgine_Texture texture;


        loadImage(instance, path, texture.image);
        texture.path = path;
        texture.type = typeName;
        texture.image.format = VK_FORMAT_A8B8G8R8_SRGB_PACK32;


        loadedTextures.push_back(texture);
        Imgine_TextureRef ref = { loadedTextures.size() - 1 };
        return ref;
    }
    else {
        return ref.value();
    }
}

void Imgine_AssetLoader::Cleanup(Imgine_Vulkan* instance)
{
    for (int i = 0; i < loadedMeshes.size(); i++) {

        loadedMeshes[i].Cleanup();
    }
    for (int i = 0; i < loadedTextures.size(); i++) {

        loadedTextures[i].Cleanup(instance);
    }
}



Imgine_TextureRef Imgine_AssetLoader::loadTexture(Imgine_Vulkan* instance, const char* path, Imgine_AssetLoader::TextureTypes type)
{
    return loadTexture(instance, path, TextureTypeNames[type]);
}


std::vector<Imgine_VulkanModel> Imgine_AssetLoader::loadModels(Imgine_Vulkan*instance, const char* path)
{
    return loadModels(instance, path, 0);//aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
}

std::vector<Imgine_VulkanModel> Imgine_AssetLoader::loadModels(Imgine_Vulkan* instance, const char* path, unsigned int flags)
{
    std::vector<Imgine_Mesh> meshes = loadMeshes(instance, path,flags);
    std::vector<Imgine_VulkanModel> models;
    for (Imgine_Mesh& mesh : meshes)
    {
        //TODO check mesh already loaded
        loadedMeshes.push_back(std::move(mesh));
        Imgine_MeshRef ref((unsigned int)(loadedMeshes.size() - 1));

        Imgine_VulkanModel model( instance,
                    loadedMeshes[loadedMeshes.size() - 1],
                    ref);


        models.push_back(std::move(model));


    }
    return models;
}
