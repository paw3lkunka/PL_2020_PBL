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

/**
 * @brief Scene saver to file class
 */
class SceneWriter
{
public:
    /**
     * @brief Construct a new Scene Writer object
     * 
     * @param objectModulePtr pointer to composition parent 
     */
    SceneWriter(ObjectModule* objectModulePtr);

    /**
     * @brief Destroy the Scene Writer object
     */
    ~SceneWriter();

    /**
     * @brief saving scene to file
     * 
     * @param filePath for saving
     */
    void saveScene(const char* filePath);

protected:
private:

    ///@brief pointer to composition parent
    ObjectModule* objModulePtr;

    ///@brief pointer to object container to make easier access
    ObjectContainer* objContainerPtr;

    ///@brief vector for saving children id for components
    std::vector<unsigned int> childrenID;

    ///@brief json object - parser to json
    nlohmann::json j;

    ///@brief saving components
    void saveBone(std::string name, Bone* componentPtr);
    void saveTransform(std::string name, Transform* componentPtr);
    void saveAudioListener(std::string name, AudioListener* componentPtr);
    void saveAudioSource(std::string name, AudioSource* componentPtr);
    void saveCamera(std::string name, Camera* componentPtr);
    void saveRenderer(std::string name, Renderer* componentPtr);
    void saveBillboardRenderer(std::string name, BillboardRenderer* componentPtr);
    void saveSphereCollider(std::string name, SphereCollider* componentPtr);

    ///@brief saving assets
    void saveMaterial(std::string name, Material* assetPtr);
    void saveTexture(std::string name, Texture* assetPtr);
    void saveMeshCustom(std::string name, MeshCustom* assetPtr);
    void saveMeshSkinned(std::string name, MeshSkinned* assetPtr);
    void saveShader(std::string name, Shader* assetPtr);

};

#endif /* !SCENEWRITER_HPP_ */
