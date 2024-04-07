#include "imgine_mesh.h"
#include "imgine_assetloader.h"
#include "imgine_define.h"
#include "imgine_vulkancommandbuffer.h"
#include  "imgine_vulkanmemoryallocator.h"
#include <assert.h>

Imgine_Mesh& Imgine_MeshRef::GetMesh()
{
#ifdef _DEBUG
	assert(ID != FALSE_ID);
#endif
	return Imgine_AssetLoader::GetInstance()->loadedMeshes[ID];
}

void Imgine_VulkanModel::Allocate(Imgine_Vulkan* instance, Imgine_Mesh& mesh)
{
    VkDeviceSize bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();

    VkBuffer stagingBuffer;
    VmaAllocation  stagingAllocation;

    createTemporaryBuffer(instance, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, stagingAllocation);

    void* mappedData;
    copyMappedMemorytoAllocation(instance, mesh.vertices.data(), stagingAllocation, static_cast<uint32_t>(mesh.vertices.size()), &mappedData);

    createBuffer(instance, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexBufferAllocation);

    copyBuffer(instance, stagingBuffer, vertexBuffer, bufferSize);

    destroyBuffer(instance, stagingBuffer, stagingAllocation);

    vertexBufferSize = mesh.vertices.size();


    VkBuffer stagingIndexBuffer;
    VmaAllocation  stagingIndexAllocation;

    VkDeviceSize bufferIndexSize = sizeof(mesh.indices[0]) * mesh.indices.size();

    createTemporaryBuffer(instance, bufferIndexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingIndexBuffer, stagingIndexAllocation);

    void* mappedIndexData;
    copyMappedMemorytoAllocation(instance, mesh.indices.data(), stagingIndexAllocation, static_cast<uint32_t>(mesh.indices.size()), &mappedIndexData);

    createBuffer(instance, bufferIndexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexBufferAllocation);

    copyBuffer(instance, stagingIndexBuffer, indexBuffer, bufferIndexSize);
    destroyBuffer(instance, stagingIndexBuffer, stagingIndexAllocation);

    indexBufferSize = mesh.indices.size();
}

void Imgine_VulkanModel::Cleanup(Imgine_Vulkan* instance)
{
	destroyBuffer(instance, vertexBuffer, vertexBufferAllocation);
	destroyBuffer(instance, indexBuffer, indexBufferAllocation);
}
