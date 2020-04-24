#ifndef _MESH_HPP
#define _MESH_HPP

#include "MeshDataStructures.inl"

class Mesh
{
public:
    /**
     * @brief Pure virtual render function for use with various render calls 
     */
    virtual void render() = 0;
    /**
     * @brief Pure virtual render function for use with instanced rendering
     */
    virtual void renderInstanced(int count) {}
};

#endif // _MESH_HPP
