#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "Component.hpp"

#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class SceneModule;

/**
 * @brief Struct that defines local transform of the object and its model matrix
 */
struct Transform : public Component
{
    friend SceneModule;

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
     * @brief  Transformation from model space to world space.
     * parentMatrix * localMatrix
     */
    const inline glm::mat4& getModelMatrix() const { return modelMatrix; }

    /**
     * @brief Transformation from parent space to world space.
     * parent->modelMatrix
     */
    const inline glm::mat4& getParentMatrix() const { return parent->modelMatrix; }

    /**
     * @brief Transformation from world space to model space.
     * modelMatrix^-1
     */
    const inline glm::mat4& getToModelMatrix() const { return toModelMatrix; }

    /**
     * @brief Transformation from world space to parent space.
     * parentMatrix^-1
     */
    const inline glm::mat4& getToParentMatrix() const { return parent->toModelMatrix; }

    /**
     * @brief Object rotation in world space.
     */
    const inline glm::quat& getWorldRotation() const { return worldRotation; }

    /**
     * @brief Set transfroms parent, as two-way pointer connection.
     * Automatically remove connection with previous parent (if existed);
     * @param newParent 
     */
    void setParent(Transform* newParent)
    {
        if(parent != nullptr)
        {
            parent->children.erase(this);
        }
        newParent->children.insert(this);
        parent = newParent;
        dirty = true;
    }

    /**
     * @brief Get pointer to Parent.
     * @return Transform& 
     */
    inline const Transform* getParent() { return parent; }

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
//serialized
    Transform* parent       = nullptr;
    glm::vec3 localPosition = {0.0f, 0.0f, 0.0f};
    glm::quat localRotation = {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 localScale    = {1.0f, 1.0f, 1.0f};
//unserialized
    glm::mat4 modelMatrix   = glm::mat4(1);
    glm::mat4 toModelMatrix = glm::mat4(1);
    glm::quat worldRotation = {1.0f, 0.0f, 0.0f, 0.0f};
};

#endif /* !TRANSFORM_HPP_ */
