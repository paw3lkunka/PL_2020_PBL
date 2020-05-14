#ifndef _MESHQUAD_HPP
#define _MESHQUAD_HPP

#include "mesh/Mesh.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

/**
 * @brief Class encapsulating standard quad rendering
 *  Used mainly for instanced rendering
 */
class MeshQuad : public Mesh
{
public:
    MeshQuad() : Mesh("", "") { setup(); }
    virtual ~MeshQuad() = default;

    /**
     * @brief Creates buffers and fills them with data
     */
    static void setup();
    
    virtual void render();
    virtual void renderInstanced(int count, glm::mat4* instanceTransforms);

private:
    static GLuint vao, vbo;
    static GLuint instanceVbo;
    static Bounds bounds;
    static float vertexData[20];
};

#endif // _MESHQUAD_HPP
