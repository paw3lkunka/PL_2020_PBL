#include "ResourceModule.hpp"
#include "FileStructures.inl"
#include "Message.inl"
#include "Core.hpp"

#include <stb_image.h>

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
            break;
            case FileType::TEXTURE:
            if(!loadTexture(fsData.path))
            {
                GetCore().getMessageBus().sendMessage(Message(Event::DEBUG_ERROR_LOG, "Can't load texture, something went wrong"));
            }
            break;
        }
    }
    else if(msg.getEvent() == Event::QUERY_MESH_DATA)
    {

    }
    else if(msg.getEvent() == Event::QUERY_TEXTURE_DATA)
    {

    }
    else if(msg.getEvent() == Event::QUERY_AUDIO_DATA)
    {

    }
    else if(msg.getEvent() == Event::QUERY_SHADER_DATA)
    {

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
    
}

bool ResourceModule::loadMesh(std::string path)
{

}

bool ResourceModule::unloadAudioClip(std::string path)
{

}

bool ResourceModule::unloadTexture(std::string path)
{

}

bool ResourceModule::unloadShader(std::string path)
{

}