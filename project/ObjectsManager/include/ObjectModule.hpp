#ifndef OBJECTMODULE_HPP_
#define OBJECTMODULE_HPP_

#include <vector>
#include <assimp/scene.h>

#include "IModule.inl"

class Entity;
class Component;
class System;
class Shader;
class Material;
enum class ComponentType;
/**
 * @brief Stores all data of Entities, Components, Shaders and materials
 * 
 */
class ObjectModule: public IModule
{
public: 
    /**
     * @brief Construct a new Object Module object
     */
    ObjectModule();
    virtual ~ObjectModule();

    /**
     * @brief inherited from IMsgReceiver
     *
     * @param msg message received
     */
    void receiveMessage(Message msg);

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
    // HACK: object allocated on heap by "new" operator - VERY temporary solution
    std::vector<Component*> components;
    /// @brief shaders container.
    std::vector<Shader> shaders;
    /// @brief materials container. 
    std::vector<Material> materials;
protected:

private:    
    /// @brief used to setting entities IDs in runtime.
    static int nextID;

    void processBones(aiScene* scene);
};

#include "ObjectsModule.ipp"

#endif /* !OBJECTMODULE_HPP_ */
