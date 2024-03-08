#define STB_IMAGE_IMPLEMENTATION
#include "imgine_stb.h"


stbi_uc* stb_Import(std::string str, int& width, int& height, int& colorDefinition)
{
    stbi_uc* pixels = stbi_load(str.c_str(), &width, &height, &colorDefinition, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    return pixels;
}

void stb_Free(stbi_uc* target)
{
    stbi_image_free(target);
}
