#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.hpp"
#include "AssetStructers.inl"
#include "Mesh.hpp"

#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>

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

    //TODO: Move STORAGES to private
    //Storages
    std::unordered_map<std::string, AudioFile> audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, Mesh> meshes;
    std::unordered_map<std::string, std::string> shaders;

private:

    /**
     * @brief assimp importer reference;
     */
    Assimp::Importer importer;

    //Storages
    std::vector<SkinnedVertex> bones;

    // Send data to MessageBus methods
    bool sendAudioClip(std::string path);
    bool sendTexture(std::string path);
    bool sendMesh(std::string path);
    bool sendShader(std::string path);
    bool sendSkinnedMesh(std::string path);

    //load files to storages methods
    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadShader(std::string path);
    bool loadMesh(std::string path);
    bool loadSkinnedMesh(std::string path);

    /**
     * @brief 
     * 
     * @param node of imported model
     * @param scene assimp structure containing nodes
     * @param path path to file to load (for saving to map purposes)
     * @return true meshes processed and saved to map
     * @return false something went wrong during processing meshes
     */
    bool processMeshNode(aiNode* node, const aiScene* scene, std::string path);
};

#endif // _RESOURCEMODULE_HPP

