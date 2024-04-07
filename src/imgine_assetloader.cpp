#include "imgine_assetloader.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include "imgine_vulkancommandbuffer.h"
#include "imgine_vulkan.h"
#include "imgine_vulkanressources.h"
#include "imgine_vulkanimage.h"
#include "imgine_assimp.h"
#include "imgine_types.h"

void loadImage(Imgine_Vulkan* instance,std::string path,VkImage* image, VmaAllocation* allocation)
{
    VkDevice device = instance->GetDevice();
    int width, height, colorDefinition;

    stbi_uc* pixels = stb_Import(path, width, height, colorDefinition);

    VkDeviceSize imageSize = width * height * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    createTextureImage(instance, imageSize,pixels,width,height, image, allocation);

    stbi_image_free(pixels);
}

std::vector<Imgine_Mesh> loadMeshes(Imgine_Vulkan* instance, std::string path)
{
    std::vector<Imgine_Mesh> meshes;
    AssimpImportScene(instance,path, meshes);
    return std::move(meshes);
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

        loadImage(instance, path, &texture.image.image, &texture.image.allocation);
        texture.path = path;
        texture.type = typeName;

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
    std::vector<Imgine_Mesh> meshes = loadMeshes(instance, path);
    std::vector<Imgine_VulkanModel> models;
    for (Imgine_Mesh& mesh : meshes)
    {
        //TODO check mesh already loaded
        loadedMeshes.push_back(mesh);

        models.push_back(std::move(Imgine_VulkanModel(instance, loadedMeshes[loadedMeshes.size() - 1])));
    }
     

    return models;
}
