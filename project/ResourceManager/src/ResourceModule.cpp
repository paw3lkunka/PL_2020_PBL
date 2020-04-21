#include "ResourceModule.hpp"
#include "FileStructures.inl"
#include "Core.hpp"
#include "Message.inl"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>
#include <glm/glm.hpp>

//TODO:Fix this couts, cerrs, because it's not proffesional

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
    std::unordered_map<std::string, Mesh>::iterator iter;
    std::cout << "Node: " << node->mName.C_Str() << " meshes: " << std::to_string(node->mNumMeshes) << std::endl;
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
        std::cout << "Loaded mesh Path: " << meshPath << "/" << node->mName.C_Str() << std::endl;
        meshes.insert(std::pair(meshPath, Mesh(vertices, indices)));
        iter = meshes.find(meshPath);

        returnFlag = returnFlag & (iter != meshes.end());
    }

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        processMeshNode(node->mChildren[i], scene, path);
    }

    return returnFlag;
}

bool ResourceModule::loadSkinnedMesh(std::string path)
{
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString();
        return false;
    }

    return processSkinnedMeshNode(scene->mRootNode, scene, path);
}

bool ResourceModule::processSkinnedMeshNode(aiNode* node, const aiScene* scene, std::string path)
{
    bool returnFlag = true;
    std::unordered_map<std::string, Mesh>::iterator iter;
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::vector<SkinnedVertex> vertices;
        std::vector<unsigned int> indices;
        std::unordered_map<std::string, BoneInfo> boneMapping;

        SkinnedVertex tempSkinnedVertex;
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
        
        unsigned int bonesAmount = 0;
        for(int j = 0; j < mesh->mNumBones; ++j)
        {
            std::string boneName(mesh->mBones[j]->mName.data);
            unsigned int boneIndex;
            if(boneMapping.find(boneName) == boneMapping.end())
            {
                BoneInfo tempBi;
                tempBi.boneIndex = bonesAmount;
                boneIndex = bonesAmount;
                tempBi.boneOffset = aiMatrixToGlmMat(mesh->mBones[i]->mOffsetMatrix);
                boneMapping[boneName] = tempBi;
                bonesAmount++;
            }
            else
            {
                boneIndex = boneMapping[boneName].boneIndex;
            }

            for(int k = 0; k < mesh->mBones[j]->mNumWeights; ++k)
            {
                unsigned int vertexID =  mesh->mBones[j]->mWeights[k].mVertexId;
                float weight = mesh->mBones[j]->mWeights[k].mWeight;
                addBoneDataToVertex(vertices[vertexID], boneIndex, weight);
            }
        }


        processIndices(indices, mesh);

        std::string meshPath = path + "/" + mesh->mName.C_Str();
        std::cout << "Loaded mesh Path: " << meshPath << std::endl;
        //meshes.insert(std::pair(meshPath, Mesh(vertices, indices)));
        //iter = meshes.find(meshPath);

        //returnFlag = returnFlag & (iter != meshes.end());
    }

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        processSkinnedMeshNode(node->mChildren[i], scene, path);
    }

    return returnFlag;
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
    std::unordered_map<std::string, Mesh>::iterator iter = meshes.find(path);

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

void ResourceModule::addBoneDataToVertex(SkinnedVertex& vertex, unsigned int& boneIndex, float& weight)
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

void ResourceModule::processAnimationNodes(aiNode* node, aiScene* scene, std::unordered_map<std::string, BoneInfo>& boneMapping, std::string filepath)
{
    std::string nodeName(node->mName.data);
    aiNodeAnim* nodeToSave;

    for(int i = 0; i < scene->mAnimations[0]->mNumChannels; ++i)
    {
        nodeToSave = scene->mAnimations[0]->mChannels[i];
        if(std::string(nodeToSave->mNodeName.data) == nodeName)
        {
            break;
        }
    }
    
}

glm::mat4 ResourceModule::aiMatrixToGlmMat(aiMatrix4x4 matrix)
{
    aiMatrix4x4 m = matrix.Transpose();

    glm::mat4 temp = glm::mat4( glm::vec4(m.a1, m.a2, m.a3, m.a4),
                                glm::vec4(m.b1, m.b2, m.b3, m.b4), 
                                glm::vec4(m.c1, m.c2, m.c3, m.c4), 
                                glm::vec4(m.d1, m.d2, m.d3, m.d4));
    std::cout   << m.a1 << " " << m.a2 << " " << m.a3 << " " << m.a4 << std::endl
                << m.b1 << " " << m.b2 << " " << m.b3 << " " << m.b4 << std::endl
                << m.c1 << " " << m.c2 << " " << m.c3 << " " << m.c4 << std::endl
                << m.d1 << " " << m.d2 << " " << m.d3 << " " << m.d4 << std::endl;
    
    std::cout   << temp[0][0] << " " << temp[0][1] << " " << temp[0][2] << " " << temp[0][3] << std::endl
                << temp[1][0] << " " << temp[1][1] << " " << temp[1][2] << " " << temp[1][3] << std::endl
                << temp[2][0] << " " << temp[2][1] << " " << temp[2][2] << " " << temp[2][3] << std::endl
                << temp[3][0] << " " << temp[3][1] << " " << temp[3][2] << " " << temp[3][3] << std::endl;
    return temp;
}

#pragma endregion