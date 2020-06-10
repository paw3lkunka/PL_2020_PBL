#ifndef BOXCOLLIDER_HPP_
#define BOXCOLLIDER_HPP_

#include "Collider.inl"

/**
 * @brief Box-shaped collider (cuboid)
 */
struct BoxCollider : public Collider
{
    /**
     * @brief Size of the collider, where:
     * x - width / 2, 
     * y - height / 2, 
     * z - depth / 2
     */
    glm::vec3 halfSize;

    /**
     * @brief Construct a new Box Collider object of size 1x1x1.
     * IMPORTANT reactCS must be still initialized by computeReactCS()!
     */
    BoxCollider();

    /**
     * @brief Construct a new Box Collider object
     * IMPORTANT reactCS must be still initialized by computeReactCS()!
     * 
     * @param size size of collider
     * @param center centre of collider (in model space), default [0,0,0]
     */
    BoxCollider(glm::vec3 size, glm::vec3 center = {0.0f, 0.0f, 0.0f});
    
    /**
     * @brief Construct a new Box Collider object
     * IMPORTANT reactCS must be still initialized by computeReactCS()!
     * 
     * @param width size of collider
     * @param height size of collider
     * @param depth size of collider
     */
    BoxCollider(float width, float height, float depth);

    /**
     * @brief Construct a new Box Collider object
     * IMPORTANT reactCS must be still initialized by computeReactCS()!
     * 
     * @param width size of collider
     * @param height size of collider
     * @param depth size of collider
     * @param x coordinate of collider centre (in model space)
     * @param y coordinate of collider centre (in model space)
     * @param z coordinate of collider centre (in model space)
     */
    BoxCollider(float width, float height, float depth, float x, float y, float z);

    //TODO documentation
    virtual ~BoxCollider();

    //TODO documentation
    virtual void computeReactCS();
};

#endif /* !BOXCOLLIDER_HPP_ */
