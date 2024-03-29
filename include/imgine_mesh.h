#pragma once
#ifndef ImGINE_MESH
#define ImGINE_MESH

#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include "imgine_glm.h"
#include "imgine_vulkanimage.h"


struct Vertex {
    Vertex() : pos(0),color(0),normal(0),tangent(0), bitangent(0),texCoord(0) {}
    Vertex(glm::vec3 pos, glm::vec3 color, glm::vec2 uv) : pos(pos), color(color), normal(0), tangent(0), bitangent(0), texCoord(uv) {}

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texCoord;

    /// <summary>
    /// Describe vertex stride and vertex/instance rate rendering
    /// </summary>
    /// <returns></returns>
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    /// <summary>
    /// Format of each params of vertex
    /// </summary>
    /// <returns></returns>
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        return attributeDescriptions;
    }
};

/// <summary>
/// Contain mesh data (Vertex,Indices,TexturesRef)
/// </summary>
struct Imgine_Mesh {

    Imgine_Mesh() {}
    Imgine_Mesh(std::vector<Vertex> vert = std::vector<Vertex>(),
        std::vector<unsigned int> inds = std::vector<unsigned int>(),
        std::vector<Imgine_TextureRef> texts = std::vector<Imgine_TextureRef>()) : vertices(vert), indices(inds), textures(texts) {}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
    std::vector<Imgine_TextureRef> textures;


    void Cleanup()
    {
        vertices.clear();
        indices.clear();
        textures.clear();
    }
};


//Id to specific loaded asset in AssetLoader
struct Imgine_MeshRef {
    unsigned int ID = FALSE_ID;
    Imgine_Mesh& GetMesh();
};

/// <summary>
/// Contain vertex and indices allocation for specific asset
/// </summary>
struct Imgine_VulkanModel {
    VkBuffer vertexBuffer;
    VmaAllocation vertexBufferAllocation;
    VkBuffer indexBuffer;
    VmaAllocation indexBufferAllocation;

    void Cleanup(Imgine_Vulkan* instance);
};




#endif