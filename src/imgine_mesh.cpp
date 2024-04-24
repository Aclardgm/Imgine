#include "imgine_mesh.h"
#include "imgine_assetloader.h"
#include "imgine_define.h"
#include "imgine_vulkancommandbuffer.h"
#include "imgine_vulkanmemoryallocator.h"
#include "imgine_vulkan.h"
#include <assert.h>

Imgine_Mesh& Imgine_MeshRef::GetMesh()
{
#ifdef _DEBUG
	assert(ID != FALSE_ID);
#endif
	return Imgine_AssetLoader::GetInstance()->loadedMeshes[ID];
}

Imgine_VulkanModel::Imgine_VulkanModel(Imgine_Vulkan* instance, Imgine_Mesh& mesh, Imgine_MeshRef meshRef)
    : vertexBuffer(
        instance,
        sizeof(mesh.vertices[0])* mesh.vertices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY),
    indexBuffer(
        instance,
        sizeof(mesh.indices[0])* mesh.indices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY),
    meshRef(meshRef)
{

    size_t vertexSize = (sizeof(mesh.vertices[0]) * mesh.vertices.size());

    Imgine_Buffer vertexStageBuffer{ instance,vertexSize , VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
    vertexStageBuffer.update(mesh.vertices.data(), vertexSize, 0);

    VkCommandBuffer commandBuffer = instance->commandBufferManager.beginSingleTimeCommand();
    copyBuffer(instance, vertexStageBuffer.buffer, vertexBuffer.buffer, vertexSize);
    
    vertexStageBuffer.Cleanup();


    size_t indexSize = (sizeof(mesh.indices[0]) * mesh.indices.size());

    Imgine_Buffer indexStageBuffer{ instance, indexSize , VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY };
    indexStageBuffer.update(mesh.indices.data(), indexSize, 0);

    commandBuffer = instance->commandBufferManager.beginSingleTimeCommand();
    copyBuffer(instance, indexStageBuffer.buffer, indexBuffer.buffer, indexSize);
    indexStageBuffer.Cleanup();
}
void Imgine_VulkanModel::Cleanup(Imgine_Vulkan* instance)
{
    vertexBuffer.Cleanup();
    indexBuffer.Cleanup();

}
