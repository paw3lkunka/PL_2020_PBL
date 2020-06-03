#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <vector>
#include <string>

#include "ISerializable.hpp"

class Component;
class SceneReader;

/**
 * @brief Representation of all objects in games, technically a collection of Components
 * 
 */
class Entity : public ISerializable
{
    friend class SceneReader;
public:
    /**
     * @brief Construct a new Entity object
     * 
     * @param id id of Entity
     * @param bufferSize allow to set initial buffer size (optional)
     * @param name of entity (optional)
     */
    Entity(int id, int bufferSize = 0, std::string name = "", bool serializable = true);

    /**
     * @brief Creates two-way pointer connection between this Entity, and pointed Component 
     * @param ptr 
     */
    void addComponent(Component* ptr);

    /**
     * @brief Delete connection between entity, and pointed component
     * 
     * @tparam T type of component (default: Component)
     * @param ptr pointer to component to detach
     * @return T* pointer to detached component, or nullptr if entity not contain this component
     */
    template<class T = Component>
    T* detachComponent(T* ptr);
    
    /**
     * @brief Delete connection between entity, and the n-th Component of given type
     * 
     * @tparam T type of component,
     * @param n occurrence of component (default: 0)
     * @return T* pointer to detached component, or nullptr if entity not contain this component
     */
    template<class T>
    T* detachComponent(int n = 0);    

    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    const int getId() const;

    /**
     * @brief Get the Name of the entity
     * 
     * @return const std::string name of entity
     */
    const std::string getName() const;

    /**
     * @brief Get vector of all components
     * 
     * @return const std::vector<Component*>* 
     */
    const std::vector<Component*>* getComponentsPtr() const;

    /**
     * @brief Get the n-th Component of given type
     * 
     * @tparam T type of component
     * @param n occurrence of component (default: 0)
     * @return const T* pointer to component (nullptr if wasn't found)
     */
    template<typename T>
    T* getComponentPtr(int n = 0) const;
    
    virtual ~Entity() = default;
protected:
private:
    /**
     * @brief ID of entity - should be unique
     */
    int id;
    /**
     * @brief contains pointers to all components associated with this entity 
     */
    std::vector<Component*> components;

    /**
     * @brief name of the entity
     */
    std::string name = "";
};

#include "Entity.ipp"

#endif /* !ENTITY_HPP_ */
