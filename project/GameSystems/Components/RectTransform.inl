#ifndef _RECTTRANSFORM_INL
#define _RECTTRANSFORM_INL

#include "Component.inl"

#include <glm/glm.hpp>
#include <unordered_set>

/**
 * @brief Defines transformations of ui elements in 2d space
 */
struct RectTransform : public Component
{
    RectTransform() = default;
    virtual ~RectTransform() = default;

    /**
     * @brief Set the rect transforms parent as two-way pointer connection.
     * Automatically remove connction with previous parent if existed.
     * @param newParent The new parent pointer to set.
     */
    void setParent(RectTransform* newParent)
    {
        if (parent != nullptr)
        {
            parent->children.erase(this);
        }
        newParent->children.insert(this);
        parent = newParent;
    }

    /**
     * @brief Get read-only reference to Local Position.
     * @return const glm::vec2& 
     */
    inline const glm::vec2 &getLocalPosition() { return localPosition; }

    /**
     * @brief Get modifiable reference to LocalPosition. 
     * Automatically sets dirty flag, use getLocalPosition for read only ops;
     * @return glm::vec2& 
     */
    inline glm::vec2 &getLocalPositionModifiable() { dirty = true; return localPosition; }

    /**
     * @brief Get read-only reference to Local Rotation.
     * @return const floatt& 
     */
    inline const float &getLocalRotation() { return localRotation; }

    /**
     * @brief Get read-only reference to Local Rotation.
     * @return const floatt& 
     */
    inline const float getLocalRotationDegrees() { return glm::degrees(localRotation); }

    /**
     * @brief Get modifiable reference to Local Rotation.
     * Automatically sets dirty flag, use getLocalRotation for read only ops;
     * @return float& 
     */
    inline float &getLocalRotationModifiable() { dirty = true; return localRotation; }

    /**
     * @brief Get read-only reference to Local Scale.
     * @return const glm::vec2& 
     */
    inline const glm::vec2 &getSize() { return size; }
    
    /**
     * @brief Get modifiable reference to Local Scale.
     * Automatically set dirty flag, use getLocalScale for read only ops; 
     * @return  glm::vec2& 
     */
    inline glm::vec2 &getSizeModifiable() { dirty = true; return size; }

    /**
     * @brief Get read-only reference to Local Scale.
     * @return const glm::vec2& 
     */
    inline const glm::vec2 &getOrigin() { return origin; }
    
    /**
     * @brief Get modifiable reference to Local Scale.
     * Automatically set dirty flag, use getLocalScale for read only ops; 
     * @return  glm::vec2& 
     */
    inline glm::vec2 &getOriginModifiable() { dirty = true; return origin; }

    /**
     * @brief Get read-only reference to Local Scale.
     * @return const glm::vec2& 
     */
    inline const glm::vec2 &getAnchor() { return anchor; }
    
    /**
     * @brief Get modifiable reference to Local Scale.
     * Automatically set dirty flag, use getLocalScale for read only ops; 
     * @return  glm::vec2& 
     */
    inline glm::vec2 &getAnchorModifiable() { dirty = true; return anchor; }

    /**
     * @brief All parent transformations combined with local transformation (localtoWorld * local)
     */
    glm::mat3 modelMatrix = glm::mat3(1.0f);

    /**
     * @brief Get the Parent object
     * @return const RectTransform* 
     */
    inline const RectTransform* getParent() { return parent; }

    /**
     * @brief Signalize if local parameters changed
     */
    bool dirty = true;

    /**
     * @brief Child nodes
     */
    std::unordered_set<RectTransform*> children;

private:
    RectTransform* parent = nullptr;
    glm::vec2 origin = {0.0f, 0.0f};
    glm::vec2 anchor = {0.0f, 0.0f};
    glm::vec2 localPosition = {0.0f, 0.0f};
    float localRotation = 0.0f;
    glm::vec2 size = {0.0f, 0.0f};
};

#endif // _RECTTRANSFORM_INL
