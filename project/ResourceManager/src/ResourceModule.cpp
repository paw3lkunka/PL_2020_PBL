#include "ResourceModule.hpp"
#include "FileStructures.inl"
#include "Core.hpp"
#include "MeshCustom.hpp"
#include "Message.inl"
#include "Transform.inl"
#include "Bone.inl"
#include "MeshDataStructures.inl"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>

#include <assimp/postprocess.h>
#include <glm/glm.hpp>

//TODO:Fix this couts, cerrs, because it's not professional

void ResourceModule::receiveMessage(Message msg)
{
    if(msg.getEvent() == Event::LOAD_FILE)
    {
        FileSystemData fsData = msg.getValue<FileSystemData>();
        switch(fsData.typeOfFile)
        {
            case FileType::AUDIO:
                if(loadAudioClip(fsData.path))
                {
                    std::cout << "Audio loaded from file " << fsData.path << std::endl;
                }
                else
                {
                    std::cerr << "Can't load audio from file " << fsData.path << std::endl;
                }
                
            break;
            case FileType::MESH:
                if(loadMesh(fsData.path))
                {
                    std::cout << "Meshes loaded from file " << fsData.path << std::endl;
                }
                else
                {
                    std::cerr << "Can't load meshes from file " << fsData.path << std::endl;
                }
            break;
            case FileType::SKINNEDMESH:
                if(loadSkinnedMesh(fsData.path))
                {
                    std::cout << "Skinned meshes loaded from file " << fsData.path << std::endl;
                }
                else
                {
                    std::cerr << "Can't load skinned meshes from file " << fsData.path << std::endl;
                }
            break;
            case FileType::SHADER:
                if(loadShader(fsData.path))
                {
                    std::cout << "Shader loaded from file " << fsData.path << std::endl;
                }
                else
                {
                    std::cerr << "Shader from file " << fsData.path << " could not be read." << std::endl;
                }
            break;
            case FileType::TEXTURE:
                if(loadTexture(fsData.path))
                {
                    std::cout << "Texture loaded from file " << fsData.path << std::endl;
                }
                else
                {
                    std::cerr << "Can't load texture from file " << fsData.path << std::endl;
                }
                
            break;
        }
    }
    else if(msg.getEvent() == Event::QUERY_MESH_DATA)
    {
        if(!sendMesh(msg.getValue<const char*>()))
        {
            std::cerr << "Can't find file " << msg.getValue<const char*>() << std::endl;
        }
    }
    else if(msg.getEvent() == Event::QUERY_TEXTURE_DATA)
    {
        if(!sendTexture(msg.getValue<const char*>()))
        {
            std::cerr << "Can't find file " << msg.getValue<const char*>() << std::endl;
        }
    }
    else if(msg.getEvent() == Event::QUERY_AUDIO_DATA)
    {
        auto filePath = msg.getValue<const char*>();
        if(!sendAudioClip(filePath))
        {
            std::cerr << "Can't find open audio file " << filePath << std::endl;

            if(loadAudioClip(filePath))
            {
                std::cout << "Audio loaded from file " << filePath << std::endl;
                sendAudioClip(filePath);
            }
            else
            {
                std::cerr << "Can't load audio from file " << filePath << std::endl;
            }
        }
    }
    else if(msg.getEvent() == Event::QUERY_SHADER_DATA)
    {
        if(!sendShader(msg.getValue<const char*>()))
        {
            std::cerr << "Can't find file " << msg.getValue<const char*>() << std::endl;
        }
    }
    else
    {
        //do nothing
    }
}

#pragma region Loaders

bool ResourceModule::loadAudioClip(std::string path)
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

bool ResourceModule::loadTexture(std::string path)
{
    TextureData tData;
    tData.data = stbi_load(path.c_str(), &tData.width, &tData.height, &tData.nrComponents, 0);
    if(tData.width != 0 && tData.height != 0 && tData.nrComponents != 0)
    {
        textures.insert( std::pair(path, tData) );
        
        std::unordered_map<std::string, TextureData>::iterator iter = textures.find(path);

        return iter != textures.end();
    }
    return false;
}

bool ResourceModule::loadShader(std::string path)
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
        return false;
    }

    shaders.insert( std::pair(path, buffer));
    std::unordered_map<std::string, std::string>::iterator iter = shaders.find(path);

    return iter != shaders.end();
}

bool ResourceModule::loadMesh(std::string path)
{   
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString();
        return false;
    }
    return processMeshNode(scene->mRootNode, scene, path);
}

bool ResourceModule::processMeshNode(aiNode* node, const aiScene* scene, std::string path)
{
    bool returnFlag = true;
    std::cout << "Node: " << node->mName.C_Str() << " meshes: " << std::to_string(node->mNumMeshes) << std::endl;
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
        meshes.insert(std::pair(meshPath, MeshCustom(vertices, indices, tBounds)));
        iter = meshes.find(meshPath);

        returnFlag = returnFlag & (iter != meshes.end());
    }

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        returnFlag = returnFlag & processMeshNode(node->mChildren[i], scene, path);
    }

    return returnFlag;
}

bool ResourceModule::loadSkinnedMesh(std::string path)
{
    // ? +++++ Load the assimp scene from provided path +++++
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
    
    // Check for assimp errors and display them
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString();
    }
    else
    {
        std::cout << "Node structure:\n";
        displayNodeHierarchy(scene->mRootNode);
        processNode(scene->mRootNode, scene, path);
    }
    // else if(processSkinnedMeshNode(scene->mRootNode, scene, path))
    // {
    //     aiNode* bonesRootNode = findRootBone(scene->mRootNode);
    //     if(bonesRootNode)
    //     {
    //         animationTicks.insert(std::pair<std::string, double>(path, scene->mAnimations[0]->mTicksPerSecond));
    //         //TODO: change this nullptr to some transform, i dunno
    //         return processBones(bonesRootNode, nullptr, scene);
    //     }
    // }

    return false;
}

void ResourceModule::processNode(aiNode* node, const aiScene* scene, std::string path, Transform* parent)
{
    // * ----- Create new entity with provided name -----
    objectModulePtr->NewEntity(scene->mMeshes[node->mMeshes[i]]->mName.C_Str(), 2);

    // * ----- Create transform and set parent -----
    Transform* nodeTransform = objectModulePtr->NewComponent<Transform>();
    nodeTransform->localToWorldMatrix = aiMatrixToGlmMat(node->mTransformation);
    nodeTransform->dirty = true;
    if (parent != nullptr)
    {
        nodeTransform->setParent(parent);
    }
    else
    {
        nodeTransform->setParent(&GetCore().sceneModule.rootNode);
    }


    // ? +++++ Process meshes (if any) ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        Mesh* newMesh = createMesh(scene->mMeshes[node->mMeshes[i]], path);

        // FIXME: If node contains more than one mesh, the entity receives more than one MeshRenderer
        // ! and mesh renderer system is not equipped to handle this

        // * ----- Create mesh renderer and add created mesh -----
        auto mr = objectModulePtr->NewComponent<MeshRenderer>();
            mr->mesh = newMesh;
    }
    
    // ? +++++ Check if node is considered a bone ++++++++++++++++++++++++++++++++++++++++++++++++
    auto bone = bones.find(path + "/" + node->mName.C_Str());

    if (bone != bones.end())
    {
        // * ----- If bone of the same name as node is found, add appropriate component to entity -----
        auto b = objectModulePtr->NewComponent<Bone>();
        b->boneIndex = bone->second.boneIndex;
        b->boneName = bone->first;
        // TODO: Check if this is the proper way to do because I'm not sure if you can just set it once
        b->inverseBindTransform = glm::inverse(parent->localToWorldMatrix * nodeTransform->localToWorldMatrix);
    }
    
    // ? +++++ Process animations (if any) ++++++++++++++++++++++++++++++++++++++++++++++++++++

    // ? +++++ Recursively call process node for all the children nodes +++++++++++++++++++++++

}

Mesh* ResourceModule::createMesh(aiMesh* mesh, std::string path)
{
    std::list<glm::vec3> positions;
    std::list<glm::vec3> normals;
    std::list<glm::vec3> tangents;
    std::list<glm::vec2> texcoords;
    struct VertexWeightInfo
    {
        float weight;
        unsigned int boneId;
    };
    std::multimap<unsigned int, VertexWeightInfo> idWeight;

    // ? +++++ Loading data for base vertex +++++
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        // * ----- Base data: position, normal, tangent -----
        positions.push_back( {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z} );
        normals.push_back( {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z} );
        tangents.push_back( {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z} );

        // * ----- Optional data: texcoords -----
        if (mesh->mTextureCoords[i])
        {
            texcoords.push_back( {mesh->mTextureCoords[i]->x, mesh->mTextureCoords[i]->y} );
        }
        else
        {
            texcoords.push_back( {0.0f, 0.0f} );
        }
    }

    // ? ----- Loading optional data: bones, boneWeights, boneIDs -----
    for (size_t i = 0; i < mesh->mNumBones; i++)
    {
        // * Author note: each loop inserts one bone into global map of bones and then saves all the weights and bone ids
        // * by vertex id to local map. The bone id is created from the size of the map to ensure uniqueness of the id,
        // * because this is the only way to map vertex to bone.
        unsigned int globalBoneIndex = bones.size();

        // XXX: I'm not sure if the insert will just do nothing if the key already exists (and that's what I want it to do) 
        // ! All bone names follow convention path/boneName to ensure uniqueness between diferrent models
        bones.insert({ path + "/" + mesh->mBones[i]->mName.C_Str(),
            BoneInfo {
                globalBoneIndex, // bone id
                aiMatrixToGlmMat(mesh->mBones[i]->mOffsetMatrix)
            }
        });

        for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            idWeight.insert({ mesh->mBones[i]->mWeights[j].mVertexId,
                VertexWeightInfo {
                    mesh->mBones[i]->mWeights[j].mWeight, // weight
                    globalBoneIndex // bone id
                }
            });
        }
    }

    // ? +++++ Loading indices data +++++
    std::vector<unsigned int> indices;
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // TODO: A lot of code is repeated here, there might be a way to make it simpler bot now I don't have the patience to bother
    // ? +++++ Check if mesh is influenced by bones +++++
    if (!mesh->HasBones())
    {
        // ? +++++ Create regular mesh and return it +++++
        std::vector<Vertex> vertices;

        std::list<glm::vec3>::iterator posIter = positions.begin();
        std::list<glm::vec3>::iterator normIter = normals.begin();
        std::list<glm::vec3>::iterator tangIter = tangents.begin();
        std::list<glm::vec2>::iterator uvIter = texcoords.begin();
        while (posIter != positions.end())
        {
            vertices.push_back(
                Vertex {
                    *posIter,
                    *normIter,
                    *tangIter,
                    *uvIter
                }
            );

            posIter++;
            normIter++;
            tangIter++;
            uvIter++;
        }

        // ? +++++ Create mesh bounds from AABB +++++
        Bounds bounds;
        bounds.maxBound = {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z};
        bounds.minBound = {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z};

        MeshCustom meshCustom(vertices, indices, bounds);

        // * ----- Add mesh to resource container -----
        meshes[path + "/" + mesh->mName.C_Str()] = meshCustom;

        // ! ===== Return appropriate mesh pointer =====
        return &meshes[path + "/" + mesh->mName.C_Str()];
    }
    else
    {
        // ? +++++ Create skinned mesh and return it +++++
        std::vector<VertexSkinned> vertices;
        
        std::list<glm::vec3>::iterator posIter = positions.begin();
        std::list<glm::vec3>::iterator normIter = normals.begin();
        std::list<glm::vec3>::iterator tangIter = tangents.begin();
        std::list<glm::vec2>::iterator uvIter = texcoords.begin();
        unsigned int i = 0;
        while (posIter != positions.end())
        {
            // * ----- Getting by the vertex id -----
            auto range = idWeight.equal_range(i);
            
            int count = std::distance(range.first, range.second);
            if (count > VertexSkinned::MAX_WEIGHTS)
            {
                // TODO: Proper error logging
                std::cerr << "Tried to insert " << count << " weights while max is " << VertexSkinned::MAX_WEIGHTS << "\n";
                std::cerr << "The excess will be clipped but the weight values will be incorrect, either increase the max weights or decrease bone influence in the model\n";
            }

            unsigned int ids[VertexSkinned::MAX_WEIGHTS];
            float weights[VertexSkinned::MAX_WEIGHTS];

            int number = VertexSkinned::MAX_WEIGHTS < count ? VertexSkinned::MAX_WEIGHTS : count; 
            for (size_t j = 0; j < number; j++)
            {
                ids[j] = range.first->second.boneId;
                weights[j] = range.first->second.weight;
                // Increment iterator
                range.first++;
            }
            

            vertices.push_back(
                VertexSkinned {
                    *posIter,
                    *normIter,
                    *tangIter,
                    *uvIter,
                    {ids[0], ids[1], ids[2], ids[3]},
                    {weights[0], weights[1], weights[2], weights[3]}
                }
            );

            posIter++;
            normIter++;
            tangIter++;
            uvIter++;
            i++;
        }

        // ? ----- Create mesh bounds from AABB -----
        Bounds bounds;
        bounds.maxBound = {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z};
        bounds.minBound = {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z};

        // ? ----- Create skinned mesh object -----
        MeshSkinned meshSkinned(vertices, indices, bounds);

        // * ----- Add mesh to resource container -----
        skinnedMeshes[path + "/" + mesh->mName.C_Str()] = meshSkinned;

        // ! ===== Return appropriate mesh pointer =====
        return &meshes[path + "/" + mesh->mName.C_Str()];
    }
}

// bool ResourceModule::processSkinnedMeshNode(aiNode* node, const aiScene* scene, std::string path)
// {
//     bool returnFlag = true;
//     std::unordered_map<std::string, MeshSkinned>::iterator iter;
//     for(int i = 0; i < node->mNumMeshes; i++)
//     {
//         aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//         aiMatrix4x4 nodeTransform = node->mTransformation;

//         std::cout << "Skinned mesh assimp info:\n";
//         std::cout << "mNumAnimations: " << scene->mNumAnimations << '\n';
//         std::cout << "mNumCameras: " << scene->mNumCameras << '\n';
//         std::cout << "mNumLights: " << scene->mNumLights << '\n';
//         std::cout << "mNumMaterials: " << scene->mNumMaterials << '\n';
//         std::cout << "mNumMeshes: " << scene->mNumMeshes << '\n';
//         std::cout << "mNumTextures: " << scene->mNumTextures << '\n';

//         std::vector<VertexSkinned> vertices;
//         std::vector<unsigned int> indices;

//         VertexSkinned tempSkinnedVertex;
//         // Read base vertex data into the structure
//         for(int j = 0; j < mesh->mNumVertices; ++j)
//         {
//             Vertex tempVertex;
//             processVertexAttributes(tempVertex, mesh, j);
//             tempSkinnedVertex.position = tempVertex.position;
//             tempSkinnedVertex.normal = tempVertex.normal;
//             tempSkinnedVertex.tangent = tempVertex.tangent;
//             tempSkinnedVertex.texcoord = tempVertex.texcoord;
            
//             vertices.push_back(tempSkinnedVertex);
//         }

//         // Read bone structure and save skinned vertex data
//         for(int j = 0; j < mesh->mNumBones; ++j)
//         {
//             // Get bone name from meshData and save as string
//             std::string boneName(mesh->mBones[j]->mName.data);
            
//             unsigned int boneIndex;
//             if(boneMapping.find(boneName) == boneMapping.end())
//             {
//                 // If the bone doesn't exist in the hierarchy add the bone to bone mapping stucture
//                 BoneInfo tempBi;
//                 tempBi.boneIndex = bonesAmount;
//                 boneIndex = bonesAmount;
//                 tempBi.boneOffset = aiMatrixToGlmMat(mesh->mBones[i]->mOffsetMatrix);
                
//                 boneMapping[boneName] = tempBi;
//                 bonesAmount++;
//                 std::cout << "Bone Name: " << boneName << std::endl; 
//             }
//             else
//             {
//                 boneIndex = boneMapping[boneName].boneIndex;
//             }

//             for(int k = 0; k < mesh->mBones[j]->mNumWeights; ++k)
//             {
//                 unsigned int vertexID = mesh->mBones[j]->mWeights[k].mVertexId;
//                 float weight = mesh->mBones[j]->mWeights[k].mWeight;
//                 addBoneDataToVertex(vertices[vertexID], boneIndex, weight);
//             }
//         }

//         processIndices(indices, mesh);

//         std::string meshPath = path + "/" + mesh->mName.C_Str();
//         std::cout << "Loaded mesh Path: " << meshPath << std::endl;
//         skinnedMeshes.insert(std::pair(meshPath, MeshSkinned(vertices, indices)));
//         iter = skinnedMeshes.find(meshPath);

//         returnFlag = returnFlag & (iter != skinnedMeshes.end());
//     }

//     for(int i = 0; i < node->mNumChildren; ++i)
//     {
//         returnFlag = returnFlag & processSkinnedMeshNode(node->mChildren[i], scene, path);
//     }

//     return returnFlag;
// }

// bool ResourceModule::processBones(aiNode* rootNode, Transform* parent, const aiScene* scene)
// {
//     bool returnFlag = true;
//     Entity* entPtr;
//     Transform* tranPtr;
//     Bone* bonePtr;

//     for(int i = 0; i < rootNode->mNumChildren; ++i)
//     {
//         aiNodeAnim* transNode = findNodeAnim(scene->mAnimations[0], rootNode->mChildren[i]->mName.C_Str());

//         if(transNode)
//         {
//             objectModulePtr->NewEntity(2);
//             std::cout << "Entity processing: " << transNode->mNodeName.C_Str() << std::endl;

//             tranPtr = objectModulePtr->NewComponent<Transform>();
//             if (parent != nullptr)
//             {
//                 tranPtr->setParent(parent);
//             }
//             else
//             {
//                 tranPtr->setParent(&GetCore().sceneModule.rootNode);
//             }

//             bonePtr = objectModulePtr->NewComponent<Bone>();
//             copyToMap(bonePtr, transNode, boneMapping[transNode->mNodeName.C_Str()].boneOffset);
//             // copyToVector(bonePtr->positionKeys, transNode->mPositionKeys, transNode->mNumPositionKeys);
//             // copyToVector(bonePtr->rotationKeys, transNode->mRotationKeys, transNode->mNumRotationKeys);
//             // copyToVector(bonePtr->scaleKeys, transNode->mScalingKeys, transNode->mNumScalingKeys);
//             bonePtr->beforeState = AnimationBehaviour((unsigned int)transNode->mPreState);
//             bonePtr->afterState = AnimationBehaviour((unsigned int)transNode->mPostState);
//             //bonePtr->offsetMatrix = boneMapping[transNode->mNodeName.C_Str()].boneOffset;
//             bonePtr->boneID = boneMapping[transNode->mNodeName.C_Str()].boneIndex;
//         }
//         returnFlag = returnFlag & processBones(rootNode->mChildren[i], new Transform(*tranPtr), scene);
//     }
//     return returnFlag;
// }

#pragma endregion

#pragma region Senders

bool ResourceModule::sendAudioClip(std::string path)
{
    std::unordered_map<std::string, AudioFile>::iterator iter = audioClips.find(path);

    if(iter != audioClips.end())
    {
        GetCore().getMessageBus().sendMessage( Message( Event::RECEIVE_AUDIO_DATA, &iter->second ) );
        return true;
    }
    
    return false;
}

bool ResourceModule::sendTexture(std::string path)
{
    std::unordered_map<std::string, TextureData>::iterator iter = textures.find(path);

    if(iter != textures.end())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::RECEIVE_TEXTURE_DATA, &iter->second) );
        return true;
    }
    return false;
}

bool ResourceModule::sendMesh(std::string path)
{
    std::unordered_map<std::string, MeshCustom>::iterator iter = meshes.find(path);

    if(iter != meshes.end())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::RECEIVE_MESH_DATA, &iter->second) );
        return true;
    }
    return false;
}

bool ResourceModule::sendShader(std::string path)
{
    std::unordered_map<std::string, std::string>::iterator iter = shaders.find(path);
    Message msgToSend;
    if(iter != shaders.end())
    {  
        msgToSend = Message(Event::RECEIVE_SHADER_DATA, iter->second.c_str());
        GetCore().getMessageBus().sendMessage( msgToSend );
        return true;
    }
    return false;
}

#pragma endregion

#pragma region Helpers

void displayNodeHierarchy(aiNode* node, int depth)
{
    for (size_t i = 0; i < depth; i++)
    {
        std::cout << '-';
    }
    std::cout << node->mName.C_Str() << '\n';

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        displayNodeHierarchy(node->mChildren[i], depth + 1);
    }
}

void displayAssimpMat4(aiMatrix4x4 mat)
{
    std::cout << mat[0][0] << '\t' << mat[1][0] << '\t' << mat[2][0] << '\t' << mat[3][0] << '\n';
    std::cout << mat[0][1] << '\t' << mat[1][1] << '\t' << mat[2][1] << '\t' << mat[3][1] << '\n'; 
    std::cout << mat[0][2] << '\t' << mat[1][2] << '\t' << mat[2][2] << '\t' << mat[3][2] << '\n'; 
    std::cout << mat[0][3] << '\t' << mat[1][3] << '\t' << mat[2][3] << '\t' << mat[3][3] << '\n'; 
}

void displayGlmMat4(glm::mat4 mat)
{
    std::cout << mat[0][0] << '\t' << mat[0][1] << '\t' << mat[0][2] << '\t' << mat[0][3] << '\n';
    std::cout << mat[1][0] << '\t' << mat[1][1] << '\t' << mat[1][2] << '\t' << mat[1][3] << '\n'; 
    std::cout << mat[2][0] << '\t' << mat[2][1] << '\t' << mat[2][2] << '\t' << mat[2][3] << '\n'; 
    std::cout << mat[3][0] << '\t' << mat[3][1] << '\t' << mat[3][2] << '\t' << mat[3][3] << '\n'; 
}

void ResourceModule::addBoneDataToVertex(VertexSkinned& vertex, unsigned int& boneIndex, float& weight)
{
    for(int i = 0; i < vertex.MAX_WEIGHTS; ++i)
    {
        if(vertex.weights[i] < 0.0001f)
        {
            vertex.boneIDs[i] = boneIndex;
            vertex.weights[i] = weight;
            return;
        }
    }
    // !If loop ends without return
    std::cerr << "ResourceModule: Too many bones for this vertex!" << std::endl;
}

void ResourceModule::processVertexAttributes(Vertex& vert, aiMesh* mesh, int vertexIndex)
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

void ResourceModule::processIndices(std::vector<unsigned int>& indices, aiMesh* mesh)
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

// aiNode* ResourceModule::findRootBone(aiNode* rootNode)
// {
//     std::unordered_map<std::string, BoneInfo>::iterator iter;
//     for(int i = 0; i < rootNode->mNumChildren; ++i)
//     {
//         iter = boneMapping.find(rootNode->mChildren[i]->mName.C_Str());
//         if(iter != boneMapping.end())
//         {
//             return rootNode->mChildren[i];
//         }
//     }

//     for(int i = 0; i < rootNode->mNumChildren; ++i)
//     {
//         return findRootBone(rootNode->mChildren[i]);
//     }
//     // !If loop ends without return
//     std::cerr << "Wrong bone name" << std::endl;
//     return nullptr;
// }

// aiNodeAnim* ResourceModule::findNodeAnim(aiAnimation* animPtr, std::string nodeName)
// {
//     for (int i = 0 ; i < animPtr->mNumChannels ; i++) {
//         aiNodeAnim* pNodeAnim = animPtr->mChannels[i];
        
//         if (std::string(pNodeAnim->mNodeName.data) == nodeName) 
//         {
//             return pNodeAnim;
//         }
//     }
    
//     return NULL;
// }

glm::mat4 ResourceModule::aiMatrixToGlmMat(aiMatrix4x4 matrix)
{
    aiMatrix4x4 m = matrix.Transpose();

    glm::mat4 temp = glm::mat4( glm::vec4(m.a1, m.a2, m.a3, m.a4),
                                glm::vec4(m.b1, m.b2, m.b3, m.b4), 
                                glm::vec4(m.c1, m.c2, m.c3, m.c4), 
                                glm::vec4(m.d1, m.d2, m.d3, m.d4));
    return temp;
}

// void ResourceModule::copyToVector(std::vector<KeyVector>& vec, aiVectorKey* tabToCopy, unsigned int size)
// {
//     aiVectorKey temp;
//     for(int i = 0; i < size; ++i)
//     {
//         temp = tabToCopy[i];
//         vec.push_back( {temp.mTime, glm::vec3(temp.mValue.x, temp.mValue.y, temp.mValue.z)} );
//     }
// }

// void ResourceModule::copyToVector(std::vector<KeyQuaternion>& vec, aiQuatKey* tabToCopy, unsigned int size)
// {
//     aiQuatKey temp;
//     for(int i = 0; i < size; ++i)
//     {
//         temp = tabToCopy[i];
//         vec.push_back( {temp.mTime, glm::quat(temp.mValue.w, temp.mValue.x, temp.mValue.y, temp.mValue.z)} );
//     }
// }

// void ResourceModule::copyToMap(Bone* bone, aiNodeAnim* animNode, glm::mat4 boneOffset)
// {
//     // ! DEBUG
//         //glm::vec3 position, scale, skew;
//         //glm::vec4 perspective;
//         //glm::quat rotation;
//         //glm::decompose(boneOffset, scale, rotation, position, skew, perspective);
//         //glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
//         // std::cout << "Bone offset matrix:\n";
//         // std::cout << boneOffset[0][0] << ' ' << boneOffset[0][1] << ' ' << boneOffset[0][2] << ' ' << boneOffset[0][3] << '\n';
//         // std::cout << boneOffset[1][0] << ' ' << boneOffset[1][1] << ' ' << boneOffset[1][2] << ' ' << boneOffset[1][3] << '\n'; 
//         // std::cout << boneOffset[2][0] << ' ' << boneOffset[2][1] << ' ' << boneOffset[2][2] << ' ' << boneOffset[2][3] << '\n'; 
//         // std::cout << boneOffset[3][0] << ' ' << boneOffset[3][1] << ' ' << boneOffset[3][2] << ' ' << boneOffset[3][3] << '\n'; 
//         // std::cout << "Decomposed bone offset pos: "  << position.x << ' ' << position.y << ' ' << position.z << '\n';
//         // std::cout << "Decomposed bone offset rot: "  << euler.x << ' ' << euler.y << ' ' << euler.z << '\n';
//     // * ===== Copy positions ====
//     for(int i = 0; i < animNode->mNumPositionKeys; ++i)
//     {
//         bone->positionKeys[animNode->mPositionKeys[i].mTime] = {animNode->mPositionKeys[i].mValue.x,
//                                                                 animNode->mPositionKeys[i].mValue.y,
//                                                                 animNode->mPositionKeys[i].mValue.z };
        
//         // float frameTime = animNode->mPositionKeys[i].mTime;
//         // //std::cout << "For frame time " << frameTime << " read position: " << bone->positionKeys[frameTime].x << ' ' << bone->positionKeys[frameTime].y << ' ' << bone->positionKeys[frameTime].z << '\n';
//         // std::cout << "For frame time " << frameTime << " read position: " << animNode->mPositionKeys[i].mValue.x << ' ' << animNode->mPositionKeys[i].mValue.y << ' ' << animNode->mPositionKeys[i].mValue.z << '\n';
//     }
//     for (int i = 0; i < animNode->mNumRotationKeys; ++i)
//     {
//         bone->rotationKeys[animNode->mRotationKeys[i].mTime] = {animNode->mRotationKeys[i].mValue.w,
//                                                                 animNode->mRotationKeys[i].mValue.x,
//                                                                 animNode->mRotationKeys[i].mValue.y,
//                                                                 animNode->mRotationKeys[i].mValue.z };
//         float frameTime = animNode->mPositionKeys[i].mTime;
//         //glm::vec3 euler = glm::degrees(glm::eulerAngles(bone->rotationKeys[frameTime]));
//         // glm::quat aaa = { animNode->mRotationKeys[i].mValue.w, animNode->mRotationKeys[i].mValue.x, animNode->mRotationKeys[i].mValue.y, animNode->mRotationKeys[i].mValue.z};
//         // glm::vec3 euler = glm::degrees(glm::eulerAngles(aaa));
//         // std::cout << "For frame time " << frameTime << " read rotation: " << euler.x << ' ' << euler.y << ' ' << euler.z << '\n';
//     }
// }

#pragma endregion