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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

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

        Bone::globalInverseTransform = glm::inverse(aiMatrixToGlmMat4(scene->mRootNode->mTransformation));
        
        processNode(scene->mRootNode, scene, path);
        processBone(scene->mRootNode, scene, path);
    }

    return false;
}

bool ResourceModule::processNode(aiNode* node, const aiScene* scene, std::string path, Transform* parent)
{
    Transform* nodeTransform = nullptr;
    // ? +++++ Process meshes (if any) ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        // * ----- Create new entity with provided name -----
        objectModulePtr->NewEntity(node->mName.C_Str(), 2);

        // * ----- Create transform and set parent -----
        nodeTransform = objectModulePtr->NewComponent<Transform>();
        
        // HACK: Decomposing matrix and setting values individually
        glm::mat4 localTransformMat = aiMatrixToGlmMat4(node->mTransformation);
        glm::vec3 position, scale, skew;
        glm::vec4 perspective;
        glm::quat rotation;
        glm::decompose(localTransformMat, scale, rotation, position, skew, perspective);

        nodeTransform->getLocalPositionModifiable() = position;
        nodeTransform->getLocalRotationModifiable() = rotation;
        nodeTransform->getLocalScaleModifiable() = scale;
        
        if (parent != nullptr)
        {
            nodeTransform->setParent(parent);
        }
        else
        {
            nodeTransform->setParent(&GetCore().sceneModule.rootNode);
        }

        Mesh* newMesh = createMesh(scene->mMeshes[node->mMeshes[i]], path);

        // FIXME: If node contains more than one mesh, the entity receives more than one MeshRenderer
        // ! and mesh renderer system is not equipped to handle this

        // * ----- Create mesh renderer and add created mesh -----
        auto mr = objectModulePtr->NewComponent<MeshRenderer>();
            mr->mesh = newMesh;
    }
    
    

    // ? +++++ Recursively call process node for all the children nodes +++++++++++++++++++++++
    // for (int i = node->mNumChildren - 1; i >= 0; i--)
    // {
    //     processNode(node->mChildren[i], scene, path, nodeTransform);
    // }
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, path, nodeTransform);
    }

    return true;
}

bool ResourceModule::processBone(aiNode* node, const aiScene* scene, std::string path, glm::mat4 parentTransform)
{
    // ? +++++ Check if node is considered a bone ++++++++++++++++++++++++++++++++++++++++++++++++
    auto bone = bones.find(path + "/" + node->mName.C_Str());
    if (bone == bones.end())
    {
        std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
    }

    glm::mat4 boneTransform = glm::mat4(1.0f);
    if (bone != bones.end())
    {
        std::cout << "Found bone: " << bone->first << " id: " << bone->second.boneIndex << '\n';
        objectModulePtr->NewEntity(node->mName.C_Str(), 1);

        boneTransform = parentTransform * aiMatrixToGlmMat4(node->mTransformation);

        // * ----- If bone of the same name as node is found, add appropriate component to entity -----
        auto b = objectModulePtr->NewComponent<Bone>();
        b->boneIndex = bone->second.boneIndex;
        b->mTransform = boneTransform;
        b->boneName = bone->first;
        b->offsetMatrix = bone->second.offsetMatrix;
    }
    
    // ? +++++ Process animations (if any) ++++++++++++++++++++++++++++++++++++++++++++++++++++
    // TODO

    // ? +++++ Recursively call process node for all the children nodes +++++++++++++++++++++++
    // for (int i = node->mNumChildren - 1; i >= 0; i--)
    // {
    //     processNode(node->mChildren[i], scene, path, nodeTransform);
    // }
    if (node->mNumChildren > 0u)
    {
        for (unsigned int i = 0u; i < node->mNumChildren; i++)
        {
            std::cout << "for(int i = 0; " << i << " < " << node->mNumChildren << " ; " << i + 1 << '\n';
            processBone(node->mChildren[i], scene, path, boneTransform);
        }
    }
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
        // TODO: Support more than one uv channel
        if (mesh->mTextureCoords[0])
        {
            texcoords.push_back( {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} );
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

        // std::cout << "Inserting bone with name: " << mesh->mBones[i]->mName.C_Str() << '\n';
        // std::cout << "++++++++++++++++++++++++++++ BONE OFFSET MATRIX ++++++++++++++++++++++++++++++++++\n";
        // displayAssimpMat4(mesh->mBones[i]->mOffsetMatrix);
        // std::cout << "++++++++++++++++++++++++++++ DECOMPOSED ++++++++++++++++++++++++++++++++++++++++++\n";
        // displayGlmMat4Decomposed(aiMatrixToGlmMat4(mesh->mBones[i]->mOffsetMatrix));

        bones.insert(
            { 
                path + "/" + mesh->mBones[i]->mName.C_Str(),
                BoneInfo 
                {
                    globalBoneIndex, // bone id
                    aiMatrixToGlmMat4(mesh->mBones[i]->mOffsetMatrix)
                }
            }
        );

        for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            idWeight.insert(
                { 
                    mesh->mBones[i]->mWeights[j].mVertexId,
                    VertexWeightInfo 
                    {
                        mesh->mBones[i]->mWeights[j].mWeight, // weight
                        globalBoneIndex // bone id
                    }
                }
            );
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

    // TODO: A lot of code is repeated here, there might be a way to make it simpler but now I don't have the patience to bother
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
        return &skinnedMeshes[path + "/" + mesh->mName.C_Str()];
    }
}

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

void displayGlmMat4Decomposed(glm::mat4 mat)
{
    glm::vec3 position, scale, skew;
    glm::vec4 perspective;
    glm::quat rotation;
    glm::decompose(mat, scale, rotation, position, skew, perspective);
    glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
    std::cout << "Position: " << position.x << ' ' << position.y << ' ' << position.z << '\n';
    std::cout << "Rotation (deg): " << euler.x << ' ' << euler.y << ' ' << euler.z << '\n';
    std::cout << "Scale: " << scale.x << ' ' << scale.y << ' ' << scale.z << '\n';
}

void displayGlmVec3(glm::vec3 vec)
{
    std::cout << vec.x << ' ' << vec.y << ' ' << vec.z << '\n';
}

void displayAssimpMat4(aiMatrix4x4 mat)
{
    std::cout << mat[0][0] << '\t' << mat[0][1] << '\t' << mat[0][2] << '\t' << mat[0][3] << '\n';
    std::cout << mat[1][0] << '\t' << mat[1][1] << '\t' << mat[1][2] << '\t' << mat[1][3] << '\n'; 
    std::cout << mat[2][0] << '\t' << mat[2][1] << '\t' << mat[2][2] << '\t' << mat[2][3] << '\n'; 
    std::cout << mat[3][0] << '\t' << mat[3][1] << '\t' << mat[3][2] << '\t' << mat[3][3] << '\n'; 
}

void displayGlmMat4(glm::mat4 mat)
{
    std::cout << mat[0][0] << '\t' << mat[1][0] << '\t' << mat[2][0] << '\t' << mat[3][0] << '\n';
    std::cout << mat[0][1] << '\t' << mat[1][1] << '\t' << mat[2][1] << '\t' << mat[3][1] << '\n'; 
    std::cout << mat[0][2] << '\t' << mat[1][2] << '\t' << mat[2][2] << '\t' << mat[3][2] << '\n'; 
    std::cout << mat[0][3] << '\t' << mat[1][3] << '\t' << mat[2][3] << '\t' << mat[3][3] << '\n'; 
}

#pragma endregion