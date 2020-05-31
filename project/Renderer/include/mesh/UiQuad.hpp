#ifndef _UIQUAD_HPP
#define _UIQUAD_HPP

#include "mesh/Mesh.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

/**
 * @brief 2d quad class for ui rendering
 */
class UiQuad : public Mesh
{
public:
    UiQuad(bool serialize = false);
    virtual ~UiQuad() = default;

    /**
     * @brief Creates buffers and fills them with data
     */
    static void setup();

    virtual void render();
    virtual void renderInstanced(int count, glm::mat4* instanceTransforms);

private:
    static bool initialized;
    static GLuint vao, vbo;
    static GLuint instanceVbo;
    static Bounds bounds;
    static float vertexData[16];
};

#endif // _UIQUAD_HPP
