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

    void receiveMessage(Message msg);

    void readScene(std::string path);

#pragma region SceneWriter Wrapper
    /**
     * @brief scene writer wrapped, saving scene
     * 
     * @param filePath path to file to save scene
     */
    void saveScene(const char* filePath);
#pragma endregion

#pragma region ObjectContainer Wrapper

    MeshCustom* getMeshCustomFromPath(const char* meshPath);

    MeshSkinned* getMeshSkinnedFromPath(const char* meshPath);

#pragma endregion

#pragma region ObjectMaker Wrapper
    /**
     * @brief (Object maker wrapper)
     * new entity making
     * 
     * @param bufferSize entity buffer size
     * @return Entity* pointer for added entity
     */
    Entity* newEntity(int bufferSize);
    
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
    void newModel(const char* filePath, FileType type);


    /**
     * @brief (Object maker wrapper)
     * make new material and save it to container
     * 
     * @param shader pointer for initialization
     * @return Material* pointer to material
     */
    Material* newMaterial(Shader* shader);

    void newAudioClip(const char* filePath);

#pragma endregion

private:    
    ObjectMaker objectMaker;
    ObjectContainer objectContainer;
    AssetReader assetReader;
    SceneReader sceneReader;
    SceneWriter sceneWriter;
};

#include "ObjectModule.ipp"

#endif /* !OBJECTMODULE_HPP_ */
