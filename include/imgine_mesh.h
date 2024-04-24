#pragma once
#ifndef ImGINE_MESH
#define ImGINE_MESH

#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include "imgine_glm.h"
#include "imgine_vulkanimage.h"
#include "imgine_vulkanbuffer.h"



struct Imgine_Vertex {
    Imgine_Vertex() : 
        pos(0),
        color(0),
        //normal(0),
        //tangent(0), 
        //bitangent(0),
        texCoord(0) {}
    Imgine_Vertex(glm::vec3 pos, glm::vec3 color, glm::vec2 uv) : 
        pos(pos), 
        color(color), 
        //normal(0), 
        //tangent(0), 
        //bitangent(0), 
        texCoord(uv) {}

    glm::vec3 pos;
    glm::vec3 color;

    //glm::vec3 normal;
    //glm::vec3 tangent;
    //glm::vec3 bitangent;

    glm::vec2 texCoord;

    /// <summary>
    /// Describe vertex stride and vertex/instance rate rendering
    /// </summary>
    /// <returns></returns>
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Imgine_Vertex);
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
        attributeDescriptions[0].offset = offsetof(Imgine_Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Imgine_Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Imgine_Vertex, texCoord);
        return attributeDescriptions;
    }
};



/// <summary>
/// Contain mesh data (Vertex,Indices,TexturesRef)
/// Mesh contains lots of data => we never allow to copy it, only moving it
/// Maybe providing method to generate a copy from existing one to allow modification on a new instance
/// </summary>
struct Imgine_Mesh {
	std::vector<Imgine_Vertex> vertices;
	std::vector<uint32_t> indices;
    std::vector<Imgine_TextureRef> textures;

    Imgine_Mesh() {}
    Imgine_Mesh(
        std::vector<Imgine_Vertex> vert,
        std::vector<uint32_t> inds,
        std::vector<Imgine_TextureRef> texts) : 
        vertices(vert), indices(inds), textures(texts) {}

    Imgine_Mesh(Imgine_Mesh&& other) :
        vertices(), indices(), textures() 
    {

        *this = std::move(other);
        other.vertices.clear();
        other.indices.clear();
        other.textures.clear();
    }
    Imgine_Mesh& operator=(Imgine_Mesh&& other)
    {
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
        return *this;
    }

    Imgine_Mesh(Imgine_Mesh& mesh) = delete;
    Imgine_Mesh& operator=(Imgine_Mesh& other) = delete;

    void Cleanup()
    {
        vertices.clear();
        indices.clear();
        textures.clear();
    }
};


//Id to specific loaded asset in AssetLoader
struct Imgine_MeshRef {
    Imgine_MeshRef(unsigned int id = FALSE_ID) : ID(id) {}
    Imgine_MeshRef(Imgine_MeshRef& ref) : ID(ref.ID) {}
    unsigned int ID = FALSE_ID;
    Imgine_Mesh& GetMesh();
};

/// <summary>
/// Contain vertex and indices allocation for specific submesh
/// and associated textures
/// </summary>
struct Imgine_VulkanModel {
    Imgine_VulkanModel(const Imgine_VulkanModel& other)
        :vertexBuffer(other.vertexBuffer),
        indexBuffer(other.indexBuffer),
        meshRef(other.meshRef.ID)
    {


    }
    Imgine_VulkanModel(Imgine_VulkanModel&& other)
        : vertexBuffer(std::move(other.vertexBuffer)),
        indexBuffer(std::move(other.indexBuffer)),
        meshRef(other.meshRef)
    {

    }
    Imgine_VulkanModel(Imgine_Vulkan* instance, Imgine_Mesh& mesh, Imgine_MeshRef meshRef);

    Imgine_MeshRef meshRef;
    Imgine_Buffer vertexBuffer;
    Imgine_Buffer indexBuffer;

    void Cleanup(Imgine_Vulkan* instance);
    Imgine_VulkanModel& operator=(Imgine_VulkanModel&& rhs) { 
        vertexBuffer = std::move(rhs.vertexBuffer);
        indexBuffer = std::move(rhs.indexBuffer);
        meshRef = rhs.meshRef;
    }
};




#endif