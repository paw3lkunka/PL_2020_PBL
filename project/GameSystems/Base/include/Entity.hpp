#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <vector>
#include <string>

class Component;

/**
 * @brief Representation of all objects in games, technically a collection of Components
 * 
 */
class Entity 
{
    public:
        std::string name = "";

        /**
         * @brief Construct a new Entity object
         * 
         * @param id id of Entity
         * @param bufferSize allow to set initial buffer size (optional)
         */
        Entity(int id, std::string name, int bufferSize);

        /**
         * @brief Creates two-way pointer connection between this Entity, and pointed Component 
         * @param ptr 
         */
        void addComponent(Component* ptr);

        /**
         * @brief Get the Id object
         * 
         * @return int 
         */
        const int getId() const;

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
};

#include "Entity.ipp"

#endif /* !ENTITY_HPP_ */
