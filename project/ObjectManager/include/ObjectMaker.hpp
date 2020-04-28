#ifndef OBJECTMAKER_HPP_
#define OBJECTMAKER_HPP_

class ObjectModule;
class ObjectContainer;

class Shader;
class Entity;
class Texture;
class Cubemap;
class Material;
struct TextureCreateInfo;

class ObjectMaker
{
public:
    ObjectMaker(ObjectModule* objectmodulePtr);
    ObjectMaker() = default;
    ~ObjectMaker() = default;

    //TODO Documentation
    Entity& newEntity(int bufferSize);
    
    /**
     * @brief Create new empty component of given type, and return pointer to it.
     * @return T* type of component;
     */
    template<typename T>
    T* newEmptyComponent();

    //TODO Documentation
    template<typename T>
    T* newEmptyComponentForLastEntity();

    Shader& newShader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = nullptr);

    Texture& newTexture(const char* filePath, TextureCreateInfo createInfo);

    Cubemap& newCubemap(TextureCreateInfo createInfo, 
                        const char* frontPath, 
                        const char* leftPath, 
                        const char* rightPath,
                        const char* backPath,
                        const char* topPath,
                        const char* bottomPath);

    // template<typename T>
    // Mesh* newMesh(const char* filePath);

protected:
private:
    /// @brief used to setting entities ID in runtime.
    static int nextID;

    static bool hasInstance;

    ObjectModule* objModPtr;
    ObjectContainer* objContainer;
};

#endif /* !OBJECTMAKER_HPP_ */
