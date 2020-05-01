#ifndef BOXCOLLIDER_HPP_
#define BOXCOLLIDER_HPP_

#include "Collider.inl"

#include <glm/glm.hpp>

/**
 * @brief Box-shaped collider (cuboid)
 */

struct BoxCollider : public Collider
{
    //TODO documentation
    void calculateVert()    
    {
        vert[0] = glm::vec4( center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1 );
        vert[1] = glm::vec4( center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z, 1 );
        vert[2] = glm::vec4( center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1 );
        vert[3] = glm::vec4( center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1 );
        vert[4] = glm::vec4( center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z, 1 );
        vert[5] = glm::vec4( center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z, 1 );
        vert[6] = glm::vec4( center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1 );
        vert[7] = glm::vec4( center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z, 1 );
    }

    //TODO documentation
    BoxCollider(glm::vec3 centre, glm::vec3 size)
    {
        this->center = center;
        this->halfSize = size * 0.5f;
        calculateVert();
    }
    
    //TODO documentation
    BoxCollider(glm::vec3 centre, float width, float height, float depth)
    {
        this->center = center;
        this->halfSize = {width * 0.5f, height * 0.5f, depth * 0.5f};
        calculateVert();
    }
    
    //TODO documentation
    BoxCollider(float x, float y, float z, float width, float height, float depth)    
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
     */
    glm::vec4 vert[8];
};

#endif /* !BOXCOLLIDER_HPP_ */
