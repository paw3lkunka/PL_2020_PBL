#ifndef _QUAD_HPP
#define _QUAD_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

class Quad
{
public:
    static constexpr int MAX_QUAD_INSTANCES = 1024;

    Quad() = default;
    virtual ~Quad() = default;

    static void setup();
    static void renderInstanced();

private:
    int id;

    static GLuint vao, vbo;
    static int count;
};

#endif // _QUAD_HPP
