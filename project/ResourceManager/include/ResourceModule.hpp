#ifndef _RESOURCEMODULE_HPP
#define _RESOURCEMODULE_HPP

#include "IModule.hpp"
#include "AudioClipAsset.inl"
#include "ModelAsset.inl"
#include "TextureAsset.inl"

#include <unordered_map>

class ResourceModule : public IModule
{
public:
    bool loadAudioClip(std::string path);
    bool loadTexture(std::string path);
    bool loadModel(std::string path);
    bool loadShader(std::string path);

    bool unloadAudioClip(std::string path);
    bool unloadTexture(std::string path);
    bool unloadModel(std::string path);
    bool unloadShader(std::string path);

    AudioClipAsset* getAudioClipAssetPtr(std::string path);
    TextureAsset* getTextureAssetPtr(std::string path);

private:
    std::unordered_map<std::string, AudioClipAsset> audioClips;
    std::unordered_map<std::string, TextureAsset> textures;
    std::unordered_map<std::string, ModelAsset> models;
    std::unordered_map<std::string, ShaderAsset> shaders;
};

#endif // _RESOURCEMODULE_HPP

