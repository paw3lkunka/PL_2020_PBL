#ifndef OBJECTMODULE_HPP_
#define OBJECTMODULE_HPP_

#include <vector>
#include <functional>
#include <assimp/scene.h>

class Entity;
class Component;
class System;
class Shader;
class Material;
class ResourceModule;
class Mesh;
struct Transform;

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
    std::vector<Component*> components;
    /// @brief shaders container.
    std::vector<Shader> shaders;
    /// @brief mesh container. 
    std::vector<Mesh> meshes;


    ///HACK: Connection between Resource Module and Object Module
    ResourceModule* resourceModulePtr;
protected:

private:    
    /// @brief used to setting entities ID in runtime.
    static int nextID;

};

#include "ObjectsModule.ipp"

#endif /* !OBJECTMODULE_HPP_ */
