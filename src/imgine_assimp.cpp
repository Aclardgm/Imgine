#include "imgine_assimp.h"
#include "imgine_mesh.h"
#include "imgine_assetloader.h"
#include "imgine_vulkan.h"
#include "imgine_types.h"


bool AssimpImportScene(
    Imgine_Vulkan* instance, 
    const std::string& pFile, 
    std::vector<Imgine_Mesh>& meshes,
    unsigned int flags) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    std::filesystem::path path = pFile;
    

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(pFile, flags);

    

    // If the import failed, report it
    if (nullptr == scene) {
        std::cout << importer.GetErrorString() << std::endl;
        return false;
    }

    //Process nodes
    processNode(instance,scene->mRootNode, scene, path, meshes);



    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}


void processNode(Imgine_Vulkan* instance, 
    aiNode* node, 
    const aiScene* scene,
    std::filesystem::path path,
    std::vector<Imgine_Mesh>& meshes) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(std::move(processMesh(instance, mesh, path, scene)));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(instance, node->mChildren[i], scene, path, meshes);
    }
}

Imgine_Mesh processMesh(
    Imgine_Vulkan* instance, 
    aiMesh* mesh,
    std::filesystem::path path,
    const aiScene* scene)
{
    // data to fill
    std::vector<Imgine_Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Imgine_TextureRef> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Imgine_Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.pos = vector;

        /*
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        
        
        */

        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec;
            vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
            // tangent
            //vector.x = mesh->mTangents[i].x;
            //vector.y = mesh->mTangents[i].y;
            //vector.z = mesh->mTangents[i].z;
            //vertex.tangent = vector;
            //// bitangent
            //vector.x = mesh->mBitangents[i].x;
            //vector.y = mesh->mBitangents[i].y;
            //vector.z = mesh->mBitangents[i].z;
            //vertex.bitangent = vector;
        }
        else
            vertex.texCoord = glm::vec2(0.0f, 0.0f);


        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Imgine_TextureRef> diffuseMapsRefs = loadMaterialTextures(instance,material, aiTextureType_DIFFUSE, path, Imgine_AssetLoader::TextureTypes::DIFFUSE);
    textures.insert(textures.end(), diffuseMapsRefs.begin(), diffuseMapsRefs.end());
    // 2. specular maps
    std::vector<Imgine_TextureRef> specularMapsRefs = loadMaterialTextures(instance, material, aiTextureType_SPECULAR, path, Imgine_AssetLoader::TextureTypes::SPECULAR);
    textures.insert(textures.end(), specularMapsRefs.begin(), specularMapsRefs.end());
    // 3. normal maps
    std::vector<Imgine_TextureRef> normalMapsRefs = loadMaterialTextures(instance, material, aiTextureType_HEIGHT, path, Imgine_AssetLoader::TextureTypes::NORMAL);
    textures.insert(textures.end(), normalMapsRefs.begin(), normalMapsRefs.end());
    // 4. height maps
    std::vector<Imgine_TextureRef> heightMapsRefs = loadMaterialTextures(instance, material, aiTextureType_AMBIENT, path, Imgine_AssetLoader::TextureTypes::HEIGHT);
    textures.insert(textures.end(), heightMapsRefs.begin(), heightMapsRefs.end());

    // return a mesh object created from the extracted mesh data
    return Imgine_Mesh(vertices, indices, textures);
}



// checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a TextureRef struct.
std::vector<Imgine_TextureRef> loadMaterialTextures(
    Imgine_Vulkan* instance, 
    aiMaterial* mat, 
    aiTextureType type,
    std::filesystem::path path,
    std::string typeName)
{
    std::vector<Imgine_TextureRef> textureRefs;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        path = path.remove_filename();
        path.append(str.C_Str());


        Imgine_AssetLoader* assetLoader = Imgine_AssetLoader::GetInstance();

        std::string _str = path.string();

        Imgine_TextureRef ref = assetLoader->loadTexture(instance, _str.c_str(), typeName);
        textureRefs.push_back(ref);
    }
    return textureRefs;
}

std::vector<Imgine_TextureRef> loadMaterialTextures(
    Imgine_Vulkan* instance, 
    aiMaterial* mat, 
    aiTextureType type,
    std::filesystem::path path,
    Imgine_AssetLoader::TextureTypes typeEnum)
{
    return loadMaterialTextures(
        instance, mat, type, path, Imgine_AssetLoader::TextureTypeNames[typeEnum]);
}
