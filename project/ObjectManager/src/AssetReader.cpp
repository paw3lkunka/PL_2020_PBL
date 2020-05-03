#include "AssetReader.hpp"
#include "FileStructures.inl"
#include "Core.hpp"
#include "MeshCustom.hpp"
#include "Message.inl"
#include "Transform.inl"
#include "Bone.inl"
#include "MeshDataStructures.inl"
#include "ObjectMaker.hpp"
#include "ObjectModule.hpp"
#include "ObjectExceptions.inl"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>
#include <string>

#include <assimp/postprocess.h>
#include <glm/glm.hpp>

//TODO:Fix this couts, cerrs, because it's not professional

bool AssetReader::hasInstance = false;

AssetReader::AssetReader(ObjectModule* objModule) : objectModulePtr(objModule) 
{
    if(hasInstance)
    {
        throw TooManyInstancesException("Asset Reader");
    }
    hasInstance = true;
}

// void AssetReader::receiveMessage(Message msg)
// {
//     if(msg.getEvent() == Event::LOAD_FILE)
//     {
//         FileSystemData fsData = msg.getValue<FileSystemData>();
//         switch(fsData.typeOfFile)
//         {
//             case FileType::AUDIO:
//                 if(loadAudioClip(fsData.path))
//                 {
//                     std::cout << "Audio loaded from file " << fsData.path << std::endl;
//                 }
//                 else
//                 {
//                     std::cerr << "Can't load audio from file " << fsData.path << std::endl;
//                 }
                
//             break;
//             case FileType::MESH:
//                 if(loadMesh(fsData.path))
//                 {
//                     std::cout << "Meshes loaded from file " << fsData.path << std::endl;
//                 }
//                 else
//                 {
//                     std::cerr << "Can't load meshes from file " << fsData.path << std::endl;
//                 }
//             break;
//             case FileType::SKINNEDMESH:
//                 if(loadSkinnedMesh(fsData.path))
//                 {
//                     std::cout << "Skinned meshes loaded from file " << fsData.path << std::endl;
//                 }
//                 else
//                 {
//                     std::cerr << "Can't load skinned meshes from file " << fsData.path << std::endl;
//                 }
//             break;
//             case FileType::SHADER:
//                 if(loadShader(fsData.path))
//                 {
//                     std::cout << "Shader loaded from file " << fsData.path << std::endl;
//                 }
//                 else
//                 {
//                     std::cerr << "Shader from file " << fsData.path << " could not be read." << std::endl;
//                 }
//             break;
//             case FileType::TEXTURE:
//                 if(loadTexture(fsData.path))
//                 {
//                     std::cout << "Texture loaded from file " << fsData.path << std::endl;
//                 }
//                 else
//                 {
//                     std::cerr << "Can't load texture from file " << fsData.path << std::endl;
//                 }
                
//             break;
//         }
//     }
//     else if(msg.getEvent() == Event::QUERY_MESH_DATA)
//     {
//         if(!sendMesh(msg.getValue<const char*>()))
//         {
//             std::cerr << "Can't find file " << msg.getValue<const char*>() << std::endl;
//         }
//     }
//     else if(msg.getEvent() == Event::QUERY_TEXTURE_DATA)
//     {
//         if(!sendTexture(msg.getValue<const char*>()))
//         {
//             std::cerr << "Can't find file " << msg.getValue<const char*>() << std::endl;
//         }
//     }
//     else if(msg.getEvent() == Event::QUERY_AUDIO_DATA)
//     {
//         auto filePath = msg.getValue<const char*>();
//         if(!sendAudioClip(filePath))
//         {
//             std::cerr << "Can't find open audio file " << filePath << std::endl;

//             if(loadAudioClip(filePath))
//             {
//                 std::cout << "Audio loaded from file " << filePath << std::endl;
//                 sendAudioClip(filePath);
//             }
//             else
//             {
//                 std::cerr << "Can't load audio from file " << filePath << std::endl;
//             }
//         }
//     }
//     else if(msg.getEvent() == Event::QUERY_SHADER_DATA)
//     {
//         if(!sendShader(msg.getValue<const char*>()))
//         {
//             std::cerr << "Can't find file " << msg.getValue<const char*>() << std::endl;
//         }
//     }
//     else
//     {
//         //do nothing
//     }
// }

#pragma region Loaders

bool AssetReader::loadAudioClip(std::string path)
{
    AudioFile audioData;
    if(audioData.load(path))
    {
        audioClips.insert( std::pair(path, audioData) );
        
        std::unordered_map<std::string, AudioFile>::iterator iter = audioClips.find(path);

        return iter != audioClips.end();
    }
    return false;
}

bool AssetReader::loadTexture(std::string path)
{
    TextureData tData;
    tData.data = stbi_load(path.c_str(), &tData.width, &tData.height, &tData.nrComponents, 0);
    if(tData.width != 0 && tData.height != 0 && tData.nrComponents != 0)
    {
        textures.insert( std::pair(path, tData) );
        
        std::unordered_map<std::string, TextureData>::iterator iter = textures.find(path);
        std::cout << "Tex loaded: " << path << std::endl;
        return iter != textures.end();
    }
    else
    {
        std::cerr << "Error while loading texture " << path << std::endl;
    }
    return false;
}

bool AssetReader::loadShader(std::string path)
{
    std::string buffer = "";
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        buffer = stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "Error while loading " << path << std::endl;
        return false;
    }

    shaders.insert( std::pair(path, buffer));
    std::unordered_map<std::string, std::string>::iterator iter = shaders.find(path);

    return iter != shaders.end();
}

#pragma region Mesh

bool AssetReader::loadMesh(std::string path)
{   
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString();
        return false;
    }
    return processMeshNode(scene->mRootNode, scene, path);
}

bool AssetReader::processMeshNode(aiNode* node, const aiScene* scene, std::string path)
{
    bool returnFlag = true;
    std::unordered_map<std::string, MeshCustom>::iterator iter;
    Bounds tBounds;
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Vertex tempVertex;
        for(int j = 0; j < mesh->mNumVertices; ++j)
        {
            processVertexAttributes(tempVertex, mesh, j);
            vertices.push_back(tempVertex);
        }

        processIndices(indices, mesh);

        std::string meshPath = path + "/" + mesh->mName.C_Str();
        std::cout << "Loaded mesh Path: " << meshPath << std::endl;
        aiVector3D tempVec = mesh->mAABB.mMin;
        tBounds.minBound = glm::vec3(tempVec.x, tempVec.y, tempVec.z);
        tempVec = mesh->mAABB.mMax;
        tBounds.maxBound = glm::vec3(tempVec.x, tempVec.y, tempVec.z);
        auto theMesh = objectModulePtr->objectMaker.newMesh<MeshCustom, Vertex>(vertices, indices, tBounds, path.c_str(), meshPath.c_str());

        returnFlag = returnFlag & (theMesh != nullptr);
    }

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        returnFlag = returnFlag & processMeshNode(node->mChildren[i], scene, path);
    }

    return returnFlag;
}

#pragma endregion

#pragma region Skinned Mesh

bool AssetReader::loadSkinnedMesh(std::string path)
{
    bonesAmount = 0;
    boneMapping.clear();
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString();
    }
    else if(processSkinnedMeshNode(scene->mRootNode, scene, path))
    {
        aiNode* bonesRootNode = findRootBone(scene->mRootNode);
        if(bonesRootNode)
        {
            animationTicks.insert(std::pair<std::string, double>(path, scene->mAnimations[0]->mTicksPerSecond));
            //TODO: change this nullptr to some transform, i dunno
            return processBones(bonesRootNode, nullptr, scene, path);
        }
    }

    return false;
}

bool AssetReader::processSkinnedMeshNode(aiNode* node, const aiScene* scene, std::string path)
{
    bool returnFlag = true;
    Bounds tBounds;
    std::unordered_map<std::string, MeshSkinned>::iterator iter;
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::vector<VertexSkinned> vertices;
        std::vector<unsigned int> indices;

        VertexSkinned tempSkinnedVertex;
        Vertex tempVertex;
        for(int j = 0; j < mesh->mNumVertices; ++j)
        {
            processVertexAttributes(tempVertex, mesh, j);
            tempSkinnedVertex.position = tempVertex.position;
            tempSkinnedVertex.normal = tempVertex.normal;
            tempSkinnedVertex.tangent = tempVertex.tangent;
            tempSkinnedVertex.texcoord = tempVertex.texcoord;
            
            vertices.push_back(tempSkinnedVertex);
        }
        
        for(int j = 0; j < mesh->mNumBones; ++j)
        {
            std::string boneName(mesh->mBones[j]->mName.data);
            unsigned int boneIndex;
            if(boneMapping.find(boneName) == boneMapping.end())
            {
                boneIndex = bonesAmount;
                boneMapping[boneName] = boneIndex;
                bonesAmount++;
            }
            else
            {
                boneIndex = boneMapping[boneName];
            }

            for(int k = 0; k < mesh->mBones[j]->mNumWeights; ++k)
            {
                unsigned int vertexID = mesh->mBones[j]->mWeights[k].mVertexId;
                float weight = mesh->mBones[j]->mWeights[k].mWeight;
                addBoneDataToVertex(vertices[vertexID], boneIndex, weight);
            }
        }

        processIndices(indices, mesh);

        std::string meshPath = path + "/" + mesh->mName.C_Str();
        std::cout << "Loaded mesh Path: " << meshPath << std::endl;
        aiVector3D tempVec = mesh->mAABB.mMin;
        tBounds.minBound = glm::vec3(tempVec.x, tempVec.y, tempVec.z);
        tempVec = mesh->mAABB.mMax;
        tBounds.maxBound = glm::vec3(tempVec.x, tempVec.y, tempVec.z);
        auto theMesh = objectModulePtr->objectMaker.newMesh<MeshSkinned, VertexSkinned>(vertices, indices, tBounds, path.c_str(), meshPath.c_str());

        returnFlag = returnFlag & (theMesh != nullptr);
    }

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        returnFlag = returnFlag & processSkinnedMeshNode(node->mChildren[i], scene, path);
    }

    return returnFlag;
}

bool AssetReader::processBones(aiNode* rootNode, Transform* parent, const aiScene* scene, std::string path)
{
    bool returnFlag = true;
    Entity* entPtr;
    Transform* tranPtr;
    Bone* bonePtr;

    for(int i = 0; i < rootNode->mNumChildren; ++i)
    {
        aiNodeAnim* transNode = findNodeAnim(scene->mAnimations[0], rootNode->mChildren[i]->mName.C_Str());

        if(transNode)
        {
            objectModulePtr->objectMaker.newEntity(2);

            tranPtr = objectModulePtr->objectMaker.newEmptyComponentForLastEntity<Transform>();
            if (parent != nullptr)
            {
                tranPtr->setParent(parent);
            }
            else
            {
                tranPtr->setParent(&(GetCore().sceneModule.rootNode));
            }

            bonePtr = objectModulePtr->objectMaker.newEmptyComponentForLastEntity<Bone>();
            copyToMap(bonePtr, transNode);
            bonePtr->filePath = path;
            bonePtr->beforeState = AnimationBehaviour((unsigned int)transNode->mPreState);
            bonePtr->afterState = AnimationBehaviour((unsigned int)transNode->mPostState);
            bonePtr->boneID = boneMapping[transNode->mNodeName.C_Str()];
        }
        returnFlag = returnFlag & processBones(rootNode->mChildren[i], new Transform(*tranPtr), scene, path);
    }
    return returnFlag;
}

#pragma endregion

#pragma endregion

#pragma region Helpers

void AssetReader::addBoneDataToVertex(VertexSkinned& vertex, unsigned int& boneIndex, float& weight)
{
    for(int i = 0; i < vertex.MAX_WEIGHTS; ++i)
    {
        if(vertex.weights[i] < 0.0001f) // if vertex has no weight on this index
        {
            vertex.boneIDs[i] = boneIndex;
            vertex.weights[i] = weight;
            return;
        }
    }
    // !If loop ends without return
    std::cerr << "AssetReader: Too many bones for this vertex!" << std::endl;
}

void AssetReader::processVertexAttributes(Vertex& vert, aiMesh* mesh, int vertexIndex)
{
    //position
    tempVector.x = mesh->mVertices[vertexIndex].x;
    tempVector.y = mesh->mVertices[vertexIndex].y;
    tempVector.z = mesh->mVertices[vertexIndex].z;
    vert.position = tempVector;

    //normals
    tempVector.x = mesh->mNormals[vertexIndex].x;
    tempVector.y = mesh->mNormals[vertexIndex].y;
    tempVector.z = mesh->mNormals[vertexIndex].z;
    vert.normal = tempVector;

    //tangent
    tempVector.x = mesh->mTangents[vertexIndex].x;
    tempVector.y = mesh->mTangents[vertexIndex].y;
    tempVector.z = mesh->mTangents[vertexIndex].z;
    vert.tangent = tempVector;

    if(mesh->mTextureCoords[0])
    {
        vert.texcoord = glm::vec2(mesh->mTextureCoords[0][vertexIndex].x, mesh->mTextureCoords[0][vertexIndex].y);
    }
    else
    {
        vert.texcoord = glm::vec2(0.0f, 0.0f);
    }
}

void AssetReader::processIndices(std::vector<unsigned int>& indices, aiMesh* mesh)
{
    for(int j = 0; j < mesh->mNumFaces; ++j)
    {
        aiFace face = mesh->mFaces[j];

        for(int k = 0; k < face.mNumIndices; ++k)
        {
            indices.push_back(face.mIndices[k]);
        }
    }
}

aiNode* AssetReader::findRootBone(aiNode* rootNode)
{
    std::unordered_map<std::string, unsigned int>::iterator iter;
    for(int i = 0; i < rootNode->mNumChildren; ++i)
    {
        iter = boneMapping.find(rootNode->mChildren[i]->mName.C_Str());
        if(iter != boneMapping.end())
        {
            return rootNode->mChildren[i];
        }
    }

    for(int i = 0; i < rootNode->mNumChildren; ++i)
    {
        return findRootBone(rootNode->mChildren[i]);
    }
    // !If loop ends without return
    std::cerr << "Wrong bone name" << std::endl;
    return nullptr;
}

aiNodeAnim* AssetReader::findNodeAnim(aiAnimation* animPtr, std::string nodeName)
{
    for (int i = 0 ; i < animPtr->mNumChannels ; i++) {
        aiNodeAnim* pNodeAnim = animPtr->mChannels[i];
        
        if (std::string(pNodeAnim->mNodeName.data) == nodeName) 
        {
            return pNodeAnim;
        }
    }
    
    return NULL;
}

glm::mat4 AssetReader::aiMatrixToGlmMat(aiMatrix4x4 matrix)
{
    aiMatrix4x4 m = matrix.Transpose();

    glm::mat4 temp = glm::mat4( glm::vec4(m.a1, m.a2, m.a3, m.a4),
                                glm::vec4(m.b1, m.b2, m.b3, m.b4), 
                                glm::vec4(m.c1, m.c2, m.c3, m.c4), 
                                glm::vec4(m.d1, m.d2, m.d3, m.d4));
    return temp;
}

void AssetReader::copyToMap(Bone* bone, aiNodeAnim* animNode)
{
    // * ===== Copy positions ====
    for(int i = 0; i < animNode->mNumPositionKeys; ++i)
    {
        bone->positionKeys[animNode->mPositionKeys[i].mTime] = {animNode->mPositionKeys[i].mValue.x,
                                                                animNode->mPositionKeys[i].mValue.y,
                                                                animNode->mPositionKeys[i].mValue.z };
    }
    for (int i = 0; i < animNode->mNumRotationKeys; ++i)
    {
        bone->rotationKeys[animNode->mRotationKeys[i].mTime] = {animNode->mRotationKeys[i].mValue.w,
                                                                animNode->mRotationKeys[i].mValue.x,
                                                                animNode->mRotationKeys[i].mValue.y,
                                                                animNode->mRotationKeys[i].mValue.z };
    }
}

#pragma endregion