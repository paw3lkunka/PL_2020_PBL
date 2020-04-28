#ifndef SCENEWRITER_HPP_
#define SCENEWRITER_HPP_

#include <json.hpp>

class ObjectModule;
class ObjectContainer;
class Component;
struct Bone;
struct Transform;
struct AudioListener;
struct AudioSource;
struct Camera;
struct Renderer;
struct BillboardRenderer;
struct SphereCollider;
class Material;
class Texture;
class MeshCustom;
class MeshSkinned;
class Shader;

class SceneWriter
{
public:
    SceneWriter(ObjectModule* objectModulePtr);
    SceneWriter() = default;
    ~SceneWriter() = default;
    void saveScene();

protected:
private:
    ObjectModule* objModulePtr;
    ObjectContainer* objContainerPtr;

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
