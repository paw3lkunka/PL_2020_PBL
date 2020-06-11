#ifndef BOXCOLLIDER_HPP_
#define BOXCOLLIDER_HPP_

#include "Collider.inl"

/**
 * @brief Box-shaped collider (cuboid)
 * IMPORTANT collider size is defined in world scale!
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
     * IMPORTANT reactShape must be still initialized by computeReactCS()!
     */
    BoxCollider();

    /**
     * @brief Construct a new Box Collider object
     * IMPORTANT reactShape must be still initialized by computeReactCS()!
     * 
     * @param size size of collider
     * @param center centre of collider (in model space), default [0,0,0]
     */
    BoxCollider(glm::vec3 size, glm::vec3 center = {0.0f, 0.0f, 0.0f});
    
    /**
     * @brief Construct a new Box Collider object
     * IMPORTANT reactShape must be still initialized by computeReactCS()!
     * 
     * @param width size of collider
     * @param height size of collider
     * @param depth size of collider
     */
    BoxCollider(float width, float height, float depth);

    /**
     * @brief Construct a new Box Collider object
     * IMPORTANT reactShape must be still initialized by computeReactCS()!
     * 
     * @param width size of collider
     * @param height size of collider
     * @param depth size of collider
     * @param x coordinate of collider centre (in model space)
     * @param y coordinate of collider centre (in model space)
     * @param z coordinate of collider centre (in model space)
     */
    BoxCollider(float width, float height, float depth, float x, float y, float z);

    /**
     * @brief Release memory allocated in physic world object.
     */
    virtual ~BoxCollider();

    /**
     * @brief Create react phisic box shape, basing on valuses stored in fields - used in physic simulation.
     */
    virtual void computeReactCS();

    /**
     * @brief Compute value of innertia tensor for given mass, basing on valuses stored in fields.
     * 
     * @param mass Mass of body
     * @return rp3d::Vector3 innertia tensor (diagonal).
     */
    virtual rp3d::Vector3 computeInnertiaTensor(float mass);
};

#endif /* !BOXCOLLIDER_HPP_ */
