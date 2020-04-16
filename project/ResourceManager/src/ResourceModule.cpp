#include "ResourceModule.hpp"
#include "FileStructures.inl"
#include "Message.inl"
#include "Core.hpp"

#include <stb_image.h>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

void ResourceModule::receiveMessage(Message msg)
{
    if(msg.getEvent() == Event::LOAD_FILE)
    {
        FileSystemData fsData = msg.getValue<FileSystemData>();
        switch(fsData.typeOfFile)
        {
            case FileType::AUDIO:
            break;
            case FileType::MESH:
            break;
            case FileType::MESH_WITH_TEXTURES:
            break;
            case FileType::SHADER:
                if(!loadShader(fsData.path))
                {
                    GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Shader %s could not be read.\n", fsData.path)) );
                }
            break;
            case FileType::TEXTURE:
                if(!loadTexture(fsData.path))
                {
                    GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Can't load texture from file %s\n", fsData.path)) );
                }
            break;
        }
    }
    else if(msg.getEvent() == Event::QUERY_MESH_DATA)
    {
        if(!sendMesh(msg.getValue<const char*>()))
        {
            GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Can't find file %s\n", msg.getValue<const char*>() )) );
        }
    }
    else if(msg.getEvent() == Event::QUERY_TEXTURE_DATA)
    {
        if(!sendTexture(msg.getValue<const char*>()))
        {
            GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Can't find file %s\n", msg.getValue<const char*>() )) );
        }
    }
    else if(msg.getEvent() == Event::QUERY_AUDIO_DATA)
    {
        if(!sendAudioClip(msg.getValue<const char*>()))
        {
            GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Can't find file %s\n", msg.getValue<const char*>() )) );
        }
    }
    else if(msg.getEvent() == Event::QUERY_SHADER_DATA)
    {
        if(!sendShader(msg.getValue<const char*>()))
        {
            GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Can't find file %s\n", msg.getValue<const char*>() )) );
        }
    }
    else
    {
        //do nothing
    }
}

bool ResourceModule::loadAudioClip(std::string path)
{
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

bool ResourceModule::loadMesh(std::string path, bool withTextures)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        GetCore().getMessageBus().sendMessage( Message(Event::DEBUG_ERROR_LOG, ("Assimp Error: %s", importer.GetErrorString())) );
        return false;
    }
    return processMeshNode(scene->mRootNode, scene);
}

bool ResourceModule::processMeshNode(aiNode* node, const aiScene* scene)
{
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

    }

    for(int i = 0; i < node->mNumChildren; i++)
    {
        processMeshNode(node->mChildren[i], scene);
    }
    return false;
}

bool ResourceModule::sendAudioClip(std::string path)
{
    std::unordered_map<std::string, AudioFile<float>>::iterator iter = audioClips.find(path);

    if(iter != audioClips.end())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::RECEIVE_AUDIO_DATA, iter->second) );
        return true;
    }
    return false;
}

bool ResourceModule::sendTexture(std::string path)
{
    std::unordered_map<std::string, TextureData>::iterator iter = textures.find(path);

    if(iter != textures.end())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::RECEIVE_TEXTURE_DATA, iter->second) );
        return true;
    }
    return false;
}

bool ResourceModule::sendMesh(std::string path)
{
    std::unordered_map<std::string, Mesh<Vertex_base>>::iterator iter = meshes.find(path);

    if(iter != meshes.end())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::RECEIVE_MESH_DATA, iter->second) );
        return true;
    }
    return false;
}

bool ResourceModule::sendShader(std::string path)
{
    std::unordered_map<std::string, std::string>::iterator iter = shaders.find(path);

    if(iter != shaders.end())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::RECEIVE_SHADER_DATA, iter->second) );
        return true;
    }
    return false;
}