#ifndef _MESHQUAD_HPP
#define _MESHQUAD_HPP

#include "Mesh.inl"
#include "MeshDataStructures.inl"

#include <glm/glm.hpp>
#include <glad/glad.h>

class MeshQuad
{
public:
    MeshQuad() = default;
    virtual ~MeshQuad() = default;

    static void setup();
    
    virtual void render();
    virtual void renderInstanced(int count);

private:
    static GLuint vao, vbo;
    static Bounds bounds;
    static float vertexData[20];
};

#endif // _MESHQUAD_HPP
