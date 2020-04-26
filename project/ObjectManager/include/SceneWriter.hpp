#ifndef SCENEWRITER_HPP_
#define SCENEWRITER_HPP_

#include "AssetReader.hpp"
#include "ObjectModule.hpp"
#include "SceneModule.hpp"

#include <json.hpp>

class Component;

class SceneWriter
{
public:
    SceneWriter() = default;
    ~SceneWriter() = default;
    void saveScene();

    AssetReader ar;
    ObjectModule om;
    SceneModule sm;
protected:
private:
    ///@brief vector for saving children id for components
    std::vector<unsigned int> childrenID;
    nlohmann::json j;

    void saveBone(std::string name, Bone* componentPtr);
    void saveTransform(std::string name, Transform* componentPtr);
    void saveAudioListener(std::string name, AudioListener* componentPtr);
    void saveAudioSource(std::string name, AudioSource* componentPtr);
    void saveCamera(std::string name, Camera* componentPtr);
    void saveRenderer(std::string name, Renderer* componentPtr);
    void saveBillboardRenderer(std::string name, BillboardRenderer* componentPtr);
    void saveSphereCollider(std::string name, SphereCollider* componentPtr);

    void saveMaterial(std::string name, Material* assetPtr);
    void saveTexture(std::string name, Texture* assetPtr);
    void saveMeshCustom(std::string name, MeshCustom* assetPtr);
    void saveMeshSkinned(std::string name, MeshSkinned* assetPtr);
    void saveShader(std::string name, Shader* assetPtr);

};

#include "SceneWriter.ipp"

#endif /* !SCENEWRITER_HPP_ */
