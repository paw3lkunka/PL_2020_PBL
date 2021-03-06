#ifndef OBJECTMAKER_HPP_
#define OBJECTMAKER_HPP_

#include <vector>
#include <string>

class ObjectModule;
class ObjectContainer;

class Shader;
class Entity;
class Texture;
class Cubemap;
class CubemapHdr;
class Material;
class Animation;
class Bone;
class Font;
enum class FileType: unsigned int;
enum class RenderType;
struct Bounds;
struct TextureCreateInfo;

class ObjectMaker
{
    friend class ObjectModule;
public:
    ObjectMaker(ObjectModule* objectmodulePtr);
    ObjectMaker() = default;
    ~ObjectMaker() = default;

    /**
     * @brief new entity making
     * 
     * @param bufferSize entity buffer size
     * @param name entity name
     * @return Entity* pointer for added entity
     */
    Entity* newEntity(int bufferSize = 0, std::string name = "");
    
    /**
     * @brief Create new empty component of given type, save to container and return pointer to it.
     * @tparam T type inheritance from Component
     * @return T* pointer to type of component;
     */
    template<typename T>
    T* newEmptyComponent();

    /**
     * @brief Create new empty component of given type, save to container, assign to last added entity and return pointer to it
     * 
     * @tparam T type inheritance from Component
     * @return T* pointer to type of component
     */
    template<typename T>
    T* newEmptyComponentForLastEntity();

    /**
     * @brief Make new shader and save to container
     * 
     * @param shaderName name of shader
     * @param vertexShaderPath path of vertex shader
     * @param fragmentShaderPath path of fragment shader
     * @param geometryShaderPath path of geometry shader (optional)
     * @return Shader* pointer to shader
     */
    Shader* newShader(std::string shaderName, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = nullptr, bool serialize = true);

    /**
     * @brief make new Texture and save to container
     * 
     * @param filePath path to texture file
     * @param createInfo basic create info (without information from file like width, height)
     * @return Texture* pointer to texture
     */
    Texture* newTexture(const char* filePath, TextureCreateInfo createInfo);

    /**
     * @brief Make new cubemap and save to container
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
     * @brief Make new cubemap and save to container
     * 
     * @param createInfo basic create info (without information from file like width, height)
     * @param cubemapPath
     * @return Cubemap* pointer to cubemap
     */
    CubemapHdr* newHdrCubemap(TextureCreateInfo createInfo, 
                            const char* frontPath, 
                            const char* leftPath, 
                            const char* rightPath, 
                            const char* backPath, 
                            const char* topPath, 
                            const char* bottomPath);

    /**
     * @brief read new meshes from model file
     * 
     * @param filePath to model
     * @param type type of meshes (skinned/ custom)
     */
    void newModel(const char* filePath, bool customName, bool createEntities = true);

    /**
     * @brief make new mesh and save it to container
     * 
     * @tparam T mesh type (skinned / custom)
     * @tparam U vertex type (normal / skinned)
     * @param vertices vector
     * @param indices vector
     * @param bounds for AABB
     * @param filePath of model
     * @param meshPath in model
     * @return T* type of mesh
     */
    template<typename T, typename U>
    T* newMesh(std::vector<U> vertices, std::vector<unsigned int> indices, Bounds bounds, std::string& filePath, std::string meshName, bool customName);

    /**
     * @brief make new material and save it to container
     * 
     * @param shader pointer for initialization
     * @param name name of material
     * @return Material* pointer to material
     */
    Material* newMaterial(Shader* shader, std::string name, RenderType renderingType, bool instancingEnalbed = false);

    /**
     * @brief Create new animation and return pointer to its object
     * 
     * @param animation animation to process
     * @param path path to file
     * @param name animation name
     * @return Animation* pointer to created object
     */
    Animation* newAnimation(Animation& animation, std::string path, std::string name);

    /**
     * @brief Create new font object and return pointer to it
     * 
     * @param filePath path to font asset
     * @param size size of the font to generate
     * @param name name of the font
     * @return Font* ponter to font
     */
    Font* newFont(const char* filePath, unsigned int size, std::string name);

protected:
private:
    /// @brief used to setting entities ID in runtime.
    static int nextID;

    static bool hasInstance;

    ObjectModule* objModPtr;
    ObjectContainer* objContainer;
};

#endif /* !OBJECTMAKER_HPP_ */
