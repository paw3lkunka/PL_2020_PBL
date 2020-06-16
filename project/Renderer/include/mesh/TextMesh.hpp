#ifndef _TEXTMESH_HPP
#define _TEXTMESH_HPP

#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

class Font;

class TextMesh : public Mesh
{
public:
    TextMesh() : Mesh("", "", false) {}
    virtual ~TextMesh();

    /**
     * @brief Create buffers
     */
    void setup();

    /**
     * @brief Render whole line of text
     */
    virtual void render();

    /**
     * @brief Text to be displayed
     */
    std::string text;

    /**
     * @brief Font with which to render
     */
    Font* font;

private:
    unsigned int vao, vbo;
};

#endif // _TEXTMESH_HPP
