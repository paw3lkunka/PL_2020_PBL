#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.inl"
#include "AudioFile.h"
#include "Model.inl"
#include "AssetStructers.inl"

#include <unordered_map>

#include <assimp/scene.h>

class ResourceModule : public IModule
{
public:
    void receiveMessage(Message msg);

private:
    std::unordered_map<std::string, AudioFile<float>> audioClips;
    std::unordered_map<std::string, TextureData> textures;
    std::unordered_map<std::string, Mesh<Vertex_base>> meshes;
    std::unordered_map<std::string, std::string> shaders;

    void sendAudioClip(std::string path);
    void sendTexture(std::string path);


    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadShader(std::string path);
    bool loadMesh(std::string path, bool withTextures = false);
    bool processMeshNode(aiNode* node, const aiScene* scene);
};

#endif // _RESOURCEMODULE_HPP

