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
class CubemapHdr;
class Shader;
class Font;
class Message;

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
    ~SceneWriter() = default;

    /**
     * @brief saving scene to file
     * 
     * @param filePath for saving
     */
    void saveScene(const char* filePath);

protected:
private:

    static bool hasInstance;
    ///@brief pointer to composition parent
    ObjectModule* objModulePtr;

    ///@brief pointer to object container to make easier access
    ObjectContainer* objContainerPtr;

    ///@brief vector for saving children id for components
    std::vector<unsigned int> childrenID;

    ///@brief map for saving children of materials
    std::unordered_map<std::string, unsigned int> childrenMap;

    ///@brief json object - parser to json
    nlohmann::json* json;

    ///@brief name of actual object
    std::string name;

    // ? saving components
    void saveTransform(Transform* componentPtr);
    void saveAudioListener(AudioListener* componentPtr);
    void saveAudioSource(AudioSource* componentPtr);
    void saveCamera(Camera* componentPtr);
    void saveMeshRenderer(MeshRenderer* componentPtr);
    void saveTerrainRenderer(TerrainRenderer* componentPtr);
    void saveSphereCollider(SphereCollider* componentPtr);
    void saveBoxCollider(BoxCollider* componentPtr);
    void saveRigidbody(Rigidbody* componentPtr);
    void saveLight(Light* componentPtr);
    void savePhysicalInputKeymap(PhysicalInputKeymap* keymapPtr);
    void savePaddle(Paddle* componentPtr);
    void saveEnemy(Enemy* enemyPtr);
    void saveEnemyAnimation(EnemyAnimation* enemyAnimPtr);
    void saveUiRenderer(UiRenderer* componentPtr);
    void saveTextRenderer(TextRenderer* componentPtr);
    void saveRectTransform(RectTransform* componentPtr);
    void saveButton(Button* componentPtr);
    void saveHydroAccelerator(HydroAccelerator* componentPtr);
    void saveUiSortingGroup(UiSortingGroup* componentPtr);
    void saveToggleButton(ToggleButton* componentPtr);
    void saveCargoButton(CargoButton* componentPtr);
    void saveCargoStorage(CargoStorage* componentPtr);

    // ! didn't call it in saveComponents
    void saveCargo(Cargo* componentPtr, std::string cargoName, nlohmann::json* parser);

    // ? saving assets
    void saveMaterial(Material* assetPtr);
    void saveTexture(Texture* assetPtr);
    void saveMesh(Mesh* assetPtr);
    void saveShader(Shader* assetPtr);
    void saveCubemap(Cubemap* assetPtr);
    void saveCubemapHdr(CubemapHdr* assetPtr);
    void saveFont(Font* assetPtr);

    // ? saving Messages
    void saveMessage(std::string msgName, Message msg, std::string typeName = "onClickEvents");
};

#endif /* !SCENEWRITER_HPP_ */
