#include "imgine_scene.h"

void Imgine_Scene::loadScene(SceneFiles targetScene)
{
	models = Imgine_AssetLoader::GetInstance()->loadModels(getVulkanInstanceBind(), scenesImports[targetScene].targetFile.c_str(), scenesImports[targetScene].flags);
}
