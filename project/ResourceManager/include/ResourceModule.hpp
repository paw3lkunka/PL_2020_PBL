#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.h"
#include "AssetStructers.inl"
#include "Mesh.hpp"

#include <unordered_map>
#include <assimp/scene.h>

class Message;

/**
 * @brief Resource Module class for reading and storage assets data
 */
class ResourceModule : public IModule
{
public:
    /**
     * @brief inherited from IModule
     * 
     * @param msg message received
     */
    void receiveMessage(Message msg);

private:
    //Storages
    std::unordered_map<std::string, AudioFile<float>> audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, Mesh> meshes;
    std::unordered_map<std::string, std::string> shaders;

    // Send data to MessageBus methods
    bool sendAudioClip(std::string path);
    bool sendTexture(std::string path);
    bool sendMesh(std::string path);
    bool sendShader(std::string path);

    //load files to storages methods
    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadShader(std::string path);
    bool loadMesh(std::string path, bool withTextures = false);
    bool processMeshNode(aiNode* node, const aiScene* scene, std::string path);
};

#endif // _RESOURCEMODULE_HPP

