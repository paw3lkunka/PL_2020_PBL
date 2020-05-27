#include "UiQuad.hpp"

#include <stdexcept>

bool UiQuad::initialized = false;
GLuint UiQuad::vao = 0;
GLuint UiQuad::vbo = 0;

Bounds UiQuad::bounds = {
    {-0.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f}
};

float UiQuad::vertexData[16] = {
    -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f
};

UiQuad::UiQuad(bool serialize) : Mesh("", "", serialize)
{
    if (!initialized)
    {
        setup();
        initialized = true;
    }
}

void UiQuad::setup()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), &vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // ===== Set instance attrib pointers =====
    // glBindBuffer(GL_ARRAY_BUFFER, instanceVboUi);
    // std::size_t vec3Size = sizeof(glm::vec3);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec3Size, (void*)0);

    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec3Size, (void*)(1 * vec3Size));
    
    // glEnableVertexAttribArray(3);
    // glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec3Size, (void*)(2 * vec3Size));

    // glVertexAttribDivisor(1, 1);
    // glVertexAttribDivisor(2, 1);
    // glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

void UiQuad::render()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void UiQuad::renderInstanced(int count, glm::mat4* instanceTransforms)
{
    throw std::runtime_error("Instanced rendering not defined for UiQuad yet!");
}