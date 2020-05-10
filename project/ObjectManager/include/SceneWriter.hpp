#ifndef SCENEWRITER_HPP_
#define SCENEWRITER_HPP_

#include <json.hpp>

#include "ComponentsPreDeclarations.hxx"

class ObjectModule;
class ObjectContainer;
class Material;
class Texture;
class Mesh;
class Cubemap;
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

    ///@brief map for saving children of materials
    std::unordered_map<std::string, unsigned int> childrenMap;

    ///@brief json object - parser to json
    nlohmann::json j;

    ///@brief saving components
    void saveTransform(std::string name, Transform* componentPtr);
    void saveAudioListener(std::string name, AudioListener* componentPtr);
    void saveAudioSource(std::string name, AudioSource* componentPtr);
    void saveCamera(std::string name, Camera* componentPtr);
    void saveMeshRenderer(std::string name, MeshRenderer* componentPtr);
    void saveBillboardRenderer(std::string name, BillboardRenderer* componentPtr);
    void saveSphereCollider(std::string name, SphereCollider* componentPtr);
    void saveBoxCollider(std::string name, BoxCollider* componentPtr);
    void saveRigidbody(std::string name, Rigidbody* componentPtr);

    ///@brief saving assets
    void saveMaterial(std::string name, Material* assetPtr);
    void saveTexture(std::string name, Texture* assetPtr);
    void saveMesh(std::string name, Mesh* assetPtr);
    void saveShader(std::string name, Shader* assetPtr);
    void saveCubemap(std::string name, Cubemap* assetPtr);
};

#endif /* !SCENEWRITER_HPP_ */
