#ifndef OBJECTMODULE_HPP_
#define OBJECTMODULE_HPP_

#include <vector>
#include <assimp/scene.h>

class Entity;
class Component;
class System;
class Shader;
class Material;
class ResourceModule;
struct Transform;

enum class ComponentType;
/**
 * @brief Stores all data of Entities, Components, Shaders and materials
 * 
 */
class ObjectModule
{
public: 
    /**
     * @brief Construct a new Object Module object
     */
    ObjectModule();
    virtual ~ObjectModule();


    /**
     * @brief Creates new Entity, and pushes it to entities vector.
     * New components will be added to it, until next invocation.
     * @param bufferSize initial size of component pointers vector.
     */
    void NewEntity(int bufferSize);
    /**
     * @brief Creates new Entity, and pushes it to entities vector.
     * New components will be added to it, until next invocation.
     * @param bufferSize initial size of component pointers vector.
     * @param name name of the entity
     */
    void NewEntity(std::string name, int bufferSize);
    /**
     * @brief Finds and returns first entity by name
     * 
     * @param name name of an entity to look for
     * @return Entity* pointer to found entity or nullptr
     */
    Entity* FindEntity(std::string name);
    /**
     * @brief Finds all entities which match a given name
     * 
     * @param name name of an entity to look for
     * @return std::vector<Entity*> Vector of pointers to entities
     */
    std::vector<Entity*> FindAllEntities(std::string name);
    
    /**
     * @brief Create new component of given type, and return pointer to it.
     * Component will be automaticaly added to last created entity in this module.
     * @return T* type of component;
     */
    template<typename T>
    T* NewComponent();

    // TODO this is still WIP section

    /// @brief entities container.
    std::vector<Entity> entities;
    /// @brief components container.
    // HACK: object allocated on heap by "new" operator - VERY temporary solution
    std::vector<Component*> components;
    /// @brief shaders container.
    std::vector<Shader> shaders;
    /// @brief materials container. 
    std::vector<Material> materials;

    ///HACK: Connection between Resource Module and Object Module
    ResourceModule* resourceModulePtr;
protected:

private:    
    /// @brief used to setting entities IDs in runtime.
    static int nextID;

};

#include "ObjectsModule.ipp"

#endif /* !OBJECTMODULE_HPP_ */
