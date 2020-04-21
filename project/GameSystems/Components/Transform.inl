#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "Component.inl"

#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * @brief Struct that defines local transform of the object and its model matrix
 */
struct Transform : public Component
{
    Transform() = default;
    virtual ~Transform() = default;

    /**
     * @brief Get read-only reference to Local Position.
     * @return const glm::vec3& 
     */
    inline const glm::vec3 &getLocalPosition() { return localPosition; }

    /**
     * @brief Get modifiable reference to LocalPosition. 
     * Automatically set dirty flag, so shouldn't be used to read value;
     * @return glm::vec3& 
     */
    inline glm::vec3 &getLocalPositionModifiable() { dirty = true; return localPosition; }

    /**
     * @brief Get read-only reference to Local Rotation.
     * @return const glm::quat& 
     */
    inline const glm::quat &getLocalRotation() { return localRotation; }

    /**
     * @brief Get modifiable reference to Local Rotation.
     * Automatically set dirty flag, so shouldn't be used to read value;
     * @return glm::quat& 
     */
    inline glm::quat &getLocalRotationModifiable() { dirty = true; return localRotation; }

    /**
     * @brief Get read-only reference to Local Scale.
     * @return const glm::vec3& 
     */
    inline const glm::vec3 &getLocalScale() { return localScale; }
    
    /**
     * @brief Get modifiable reference to Local Scale.
     * Automatically set dirty flag, so shouldn't be used to read value; 
     * @return  glm::vec3& 
     */
    inline glm::vec3 &getLocalScaleModifiable() { dirty = true; return localScale; }

    /**
     * @brief Set transfroms parent, as two-way pointer connection.
     * Automatically remove connection with previous parent (if existed);
     * @param newParent 
     */
    void setParent(Transform* newParent)
    {
        if(parent)
        {
            parent->children.erase(this);
        }

        newParent->children.insert(this);
        parent = newParent;
    }

    /**
     * @brief Get pointer to Parent.
     * @return Transform& 
     */
    inline const Transform* getParent() { return parent; }

    /**
     * @brief Model matrix (local to world transform)
     */
    glm::mat4 localToWorldMatrix = glm::mat4(1);
    
    /**
     * @brief Model matrix^-1 (world to local transform)
     */
    glm::mat4 worldToLocalMatrix = glm::mat4(1);

    /**
     * @brief Signalize, if local parameters was changed.
     * system chaning localPosition, localRotation or localScale should also set it true.
     */
    bool dirty = true;

    /**
     * @brief child nodes.
     */
    std::unordered_set<Transform*> children;

private:
    Transform* parent = nullptr;
    glm::vec3 localPosition = {0,0,0};
    glm::quat localRotation = {1,0,0,0};
    glm::vec3 localScale = {1,1,1};
};

#endif /* !TRANSFORM_HPP_ */
