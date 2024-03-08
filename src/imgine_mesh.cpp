#include "imgine_mesh.h"
#include "imgine_assetloader.h"
#include "imgine_define.h"
#include "imgine_vulkancommandbuffer.h"
#include <assert.h>

Imgine_Mesh& Imgine_MeshRef::GetMesh()
{
#ifdef _DEBUG
	assert(ID != FALSE_ID);
#endif
	return Imgine_AssetLoader::GetInstance()->loadedMeshes[ID];
}

void Imgine_VulkanModel::Cleanup(Imgine_Vulkan* instance)
{
	destroyBuffer(instance, vertexBuffer, vertexBufferAllocation);
	destroyBuffer(instance, indexBuffer, indexBufferAllocation);
}
