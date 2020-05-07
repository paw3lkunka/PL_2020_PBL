#ifndef OBJECTCONTAINER_HPP_
#define OBJECTCONTAINER_HPP_

#include <vector>
#include <map>
#include <unordered_map>

#include "Bone.inl"
#include "Animation.hpp"

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

/**
 * @brief container of entity, components and assets
 */
class ObjectContainer
{
    friend class ObjectMaker;
    friend class ObjectModule;
    friend class SceneWriter;
public:
    /**
     * @brief Construct a new Object Container object
     * 
     * @param objModule pointer to object module
     */
    ObjectContainer(ObjectModule* objModule);

    /**
     * @brief Destroy the Object Container
     */
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

    /**
     * @brief Get the Mesh by Serialization id 
     * 
     * @param serializationID id of mesh
     * @return Mesh* mesh pointer or nullprt if can't find
     */
    Mesh* getMeshFromSerializationID(unsigned int serializationID);
    
    /**
     * @brief Get the Entity by ID
     * 
     * @param entityID id of entity to find
     * @return Entity* entity pointer or nullptr if can't find
     */
    Entity* getEntityFromID(unsigned int entityID);

    /**
     * @brief Get the Entity by serialization ID
     * 
     * @param serializationID of entity
     * @return Entity* pointer or nullptr if can't find
     */
    Entity* getEntityFromSerializationID(unsigned int serializationID);

    /**
     * @brief Get the Entity by name
     * 
     * @param name of entity
     * @return Entity* pointer or nullptr if can't find
     */
    Entity* getEntityFromName(const char* name);

    /**
     * @brief Get the Component by serialization ID
     * 
     * @param serializationID of component
     * @return Component* pointer or nullptr if can't find
     */
    Component* getComponentFromSerializationID(unsigned int serializationID);

    /**
     * @brief Get the Shader by serialization ID
     * 
     * @param serializationID of shader
     * @return Shader* pointer or nullptr if can't find
     */
    Shader* getShaderFromSerializationID(unsigned int serializationID);

    /**
     * @brief Get the Texture by serialization ID
     * 
     * @param serializationID of texture
     * @return Texture* pointer or nullptr if can't find
     */
    Texture* getTextureFromSerializationID(unsigned int serializationID);

    /**
     * @brief Get the Cubemap by serialization ID 
     * 
     * @param serializationID of cubemap
     * @return Cubemap* pointer or nullptr if can't find
     */
    Cubemap* getCubemapFromSerializationID(unsigned int serializationID);

    /**
     * @brief Get the Material by serializaion ID
     * 
     * @param serializationID of material
     * @return Material* pointer or nullptr if can't find
     */
    Material* getMaterialFromSerializationID(unsigned int serializationID);
    /**
     * @brief Get the Material by name
     * 
     * @param name of material
     * @return Material* pointer or nullptr if can't find
     */
    Material* getMaterialFromName(const char* name);

    // TODO documentation 
    Bone* getBonePtrByName(const char* name);
    unsigned int getBoneCount();
    // TODO documentation 
    std::unordered_map<std::string, Bone>::iterator getBoneIterByName(const char* name);
    // TODO documentation
    Animation* getAnimationPtrByName(const char* name);

protected:
private:
    ///@brief object module pointer
    ObjectModule* objModule;

    ///@brief single instance flag
    static bool hasInstance;

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
    /**
     * @brief Bones data collection
     * @key path/bone_name
     */
    std::unordered_map<std::string, Bone> bones;
    /**
     * @brief Anitmations data collection
     * @key path/animation_name
     */
    // TODO Convert into unoredered map
    std::map<std::string, Animation> animations;
};

#endif /* !OBJECTCONTAINER_HPP_ */
