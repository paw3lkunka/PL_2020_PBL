#ifndef OBJECTMODULE_HPP_
#define OBJECTMODULE_HPP_

#include <vector>
#include <assimp/scene.h>

class Entity;
class Shader;
class Texture;
class Cubemap;
class Material;
enum class FileType: unsigned int;
enum class RenderType;
struct Bounds;
struct TextureCreateInfo;
class Message;

#include "ObjectContainer.hpp"
#include "ObjectMaker.hpp"
#include "AssetReader.hpp"
#include "SceneReader.hpp"
#include "SceneWriter.hpp"
#include "IModule.inl"

/**
 * @brief Stores all data of Entities, Components, Shaders and materials
 * 
 */
class ObjectModule : public IModule
{
    friend class ObjectMaker;
    friend class SceneWriter;
    friend class SceneReader;
    friend class AssetReader;
public: 
    /**
     * @brief Construct a new Object Module object
     */
    ObjectModule() : objectMaker(this), objectContainer(this), assetReader(this), sceneWriter(this), sceneReader(this) {}
    virtual ~ObjectModule() = default;

    /**
     * @brief Get the Entities Vector object from container
     * 
     * @return std::vector<Entity>* pointer to vector
     */
    std::vector<Entity>* getEntitiesVector();

    /**
     * @brief Inherited from IModule
     * 
     * @param msg message to receive
     */
    void receiveMessage(Message msg);

    /**
     * @brief comparsion cstrings
     * 
     * @param str1 first string
     * @param str2 second string
     * @return true strings are the same
     * @return false strings aren't the same
     */
    bool compareStrings(const char* str1, const char* str2);

#pragma region Scene Wrapper
    /**
     * @brief scene writer wrapped, saving scene
     * 
     * @param filePath path to file to save scene
     */
    void saveScene(const char* filePath);

    /**
     * @brief read scene to file
     * 
     * @param path path to json file to read
     */
    void readScene(std::string path);

#pragma endregion

#pragma region ObjectContainer Wrapper

    /**
     * @brief Get the Mesh Custom From Path 
     * 
     * @param meshPath from file
     * @return MeshCustom* object if found, else throws FileNotFoundException
     */
    MeshCustom* getMeshCustomFromPath(const char* meshPath) { return objectContainer.getMeshCustomFromPath(meshPath); }

    /**
     * @brief Get the Mesh Skinned From Path
     * 
     * @param meshPath from model file
     * @return MeshSkinned* object if found, else throws FileNotFoundException
     */
    MeshSkinned* getMeshSkinnedFromPath(const char* meshPath) { return objectContainer.getMeshSkinnedFromPath(meshPath); }
    
    /**
     * @brief Get the Entity by ID
     * 
     * @param entityID id of entity to find
     * @return Entity* entity pointer or nullptr if can't find
     */
    Entity* getEntityFromID(unsigned int entityID) { return objectContainer.getEntityFromID(entityID); }

    /**
     * @brief Get the Entity by name
     * 
     * @param name of entity
     * @return Entity* pointer or nullptr if can't find
     */
    Entity* getEntityFromName(const char* name) { return objectContainer.getEntityFromName(name); }

    /**
     * @brief Get the Material by name
     * 
     * @param name of material
     * @return Material* pointer or nullptr if can't find
     */
    Material* getMaterialFromName(const char* name) {return objectContainer.getMaterialFromName(name); }

    // TODO documentation
    Bone* getBonePtrByName(const char* name);
    // TODO documentation
    Animation* getAnimationPtrByName(const char* name);

#pragma endregion

#pragma region ObjectMaker Wrapper
    /**
     * @brief (Object maker wrapper)
     * new entity making
     * 
     * @param bufferSize entity buffer size
     * @param name of the entity (optional)
     * @return Entity* pointer for added entity
     */
    Entity* newEntity(int bufferSize, std::string name = "");
    
    /**
     * @brief (Object maker wrapper)
     * Create new empty component of given type, save to container and return pointer to it.
     * @tparam T type inheritance from Component
     * @return T* pointer to type of component;
     */
    template<typename T>
    T* newEmptyComponent();

    /**
     * @brief (Object maker wrapper)
     * Create new empty component of given type, save to container, assign to last added entity and return pointer to it
     * 
     * @tparam T type inheritance from Component
     * @return T* pointer to type of component
     */
    template<typename T>
    T* newEmptyComponentForLastEntity();

    /**
     * @brief Checks if shader exist, if doesn't - makes it
     * 
     * @param vertexShaderPath path of vertex shader
     * @param fragmentShaderPath path of fragment shader
     * @param geometryShaderPath path of geometry shader (optional)
     * @return Shader* pointer to shader
     */
    Shader* newShader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = nullptr);

    /**
     * @brief Checks if texture exist, if doesn't - makes it
     * 
     * @param filePath path to texture file
     * @param createInfo basic create info (without information from file like width, height)
     * @return Texture* pointer to texture
     */
    Texture* newTexture(const char* filePath, TextureCreateInfo createInfo);

    /**
     * @brief Checks if cubemap exist, if doesn't - makes it
     * 
     * @param createInfo basic create info (without information from file like width, height)
     * @param frontPath front wall path
     * @param leftPath left wall path
     * @param rightPath right wall path
     * @param backPath back wall path
     * @param topPath top wall path
     * @param bottomPath bottom wall path
     * @return Cubemap* pointer to cubemap
     */
    Cubemap* newCubemap(TextureCreateInfo createInfo, 
                        const char* frontPath, 
                        const char* leftPath, 
                        const char* rightPath,
                        const char* backPath,
                        const char* topPath,
                        const char* bottomPath);

    /**
     * @brief Checks if model was loaded before, if doesn't - loads it
     * 
     * @param filePath to model
     * @param type type of meshes (skinned/ custom)
     */
    void newModel(const char* filePath);


    /**
     * @brief (Object maker wrapper)
     * make new material and save it to container
     * 
     * @param shader pointer for initialization
     * @return Material* pointer to material
     */
    Material* newMaterial(Shader* shader, std::string name, RenderType renderingType, bool instancingEnabled = false);

    /**
     * @brief creating new audio clip and saving to map
     * 
     * @param filePath to audio file
     */
    void newAudioClip(const char* filePath);

#pragma endregion

private:    
    ObjectMaker objectMaker;
    ObjectContainer objectContainer;
    AssetReader assetReader;
    SceneReader sceneReader;
    SceneWriter sceneWriter;

    
    bool compareStrings(std::string str1, std::string str2);
    
};

#include "ObjectModule.ipp"

#endif /* !OBJECTMODULE_HPP_ */
