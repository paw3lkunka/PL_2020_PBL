#ifndef OBJECTCONTAINER_HPP_
#define OBJECTCONTAINER_HPP_

#include <vector>

class Entity;
class Component;
class Shader;
class Mesh;
class MeshSkinned;
class MeshCustom;
class Texture;
class Cubemap;
class Material;

class ObjectMaker;
class ObjectModule;
class SceneWriter;

class ObjectContainer
{
    friend class ObjectMaker;
    friend class ObjectModule;
    friend class SceneWriter;
public:
    ObjectContainer(ObjectModule* objModule);
    ObjectContainer();
    ~ObjectContainer();

    /**
     * @brief Get the Mesh Custom From Path 
     * 
     * @param meshPath from file
     * @return MeshCustom* object if found, else throws FileNotFoundException
     */
    MeshCustom* getMeshCustomFromPath(const char* meshPath);

    /**
     * @brief Get the Mesh Skinned From Path
     * 
     * @param meshPath from model file
     * @return MeshSkinned* object if found, else throws FileNotFoundException
     */
    MeshSkinned* getMeshSkinnedFromPath(const char* meshPath);
    
    Entity* getEntityFromID(unsigned int entityID);
    Entity* getEntityFromSerializationID(unsigned int serializationID);
    Component* getComponentFromSerializationID(unsigned int serializationID);
    Shader* getShaderFromSerializationID(unsigned int serializationID);
    Mesh* getMeshFromSerializationID(unsigned int serializationID);
    Texture* getTextureFromSerializationID(unsigned int serializationID);
    Cubemap* getCubemapFromSerializationID(unsigned int serializationID);
    Material* getMaterialFromSerializationID(unsigned int serializationID);

protected:
private:
    ObjectModule* objModule;

    /// @brief entities container.
    std::vector<Entity> entities;
    /// @brief components container.
    std::vector<Component*> components;
    /// @brief shaders container.
    std::vector<Shader*> shaders;
    /// @brief mesh container. 
    std::vector<Mesh*> meshes;
    /// @brief texture container. 
    std::vector<Texture*> textures;
    ///@brief cubemaps constainer.
    std::vector<Cubemap*> cubemaps;
    ///@brief materials constainer.
    std::vector<Material*> materials;
};

#endif /* !OBJECTCONTAINER_HPP_ */
