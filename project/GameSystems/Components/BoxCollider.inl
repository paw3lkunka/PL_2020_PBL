#ifndef BOXCOLLIDER_HPP_
#define BOXCOLLIDER_HPP_

#include "Collider.inl"

#include <glm/glm.hpp>

/**
 * @brief Box-shaped collider (cuboid)
 * Important: colliders ignore local scale
 */

struct BoxCollider : public Collider
{
    /**
     * @brief Calculate all vertices coordinates (in model space), should ba called after parameters change  
     */
    void calculateVert()    
    {
        verts[0] = glm::vec4( center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1 );
        verts[1] = glm::vec4( center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1 );
        verts[2] = glm::vec4( center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1 );
        verts[3] = glm::vec4( center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1 );
        verts[4] = glm::vec4( center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1 );
        verts[5] = glm::vec4( center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1 );
        verts[6] = glm::vec4( center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1 );
        verts[7] = glm::vec4( center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1 );
    }

    /**
     * @brief Construct a new Box Collider object
     * State of object is nivalid, ans should be  
     */
    BoxCollider() = default;

    /**
     * @brief Construct a new Box Collider object
     * 
     * @param size size of collider
     * @param center centre of collider (in model space), default [0,0,0]
     */
    BoxCollider(glm::vec3 size, glm::vec3 center = {0.0f, 0.0f, 0.0f})
    {
        this->center = center;
        this->halfSize = size * 0.5f;
        calculateVert();
    }
    
    /**
     * @brief Construct a new Box Collider object
     * 
     * @param width size of collider
     * @param height size of collider
     * @param depth size of collider
     */
    BoxCollider(float width, float height, float depth)    
    {
        this->center = {0.0f, 0.0f, 0.0f};
        this->halfSize = {width * 0.5f, height * 0.5f, depth * 0.5f};
        calculateVert();
    }
    
    /**
     * @brief Construct a new Box Collider object
     * 
     * @param width size of collider
     * @param height size of collider
     * @param depth size of collider
     * @param x coordinate of collider centre (in model space)
     * @param y coordinate of collider centre (in model space)
     * @param z coordinate of collider centre (in model space)
     */
    BoxCollider(float width, float height, float depth, float x, float y, float z)    
    {
        this->center = {x,y,z};
        this->halfSize = {width * 0.5f, height * 0.5f, depth * 0.5f};
        calculateVert();
    }
    virtual ~BoxCollider() = default;

    /**
     * @brief Center of the collider
     */
    glm::vec3 center;

    /**
     * @brief Size of the collider, where:
     * x - width / 2, 
     * y - height / 2, 
     * z - depth / 2
     */
    glm::vec3 halfSize;

    /**
     * @brief Vertices of bounding box.
     * x, y, z - carteian coordinates.
     * w - always 1.
     * generated automatically by calculateVert() call.
     */
    glm::vec4 verts[8];
};

#endif /* !BOXCOLLIDER_HPP_ */
