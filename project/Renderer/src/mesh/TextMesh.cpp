#include "TextMesh.hpp"

#include "Font.hpp"

void TextMesh::setup()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextMesh::render()
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    float x = 0.0f;
    float y = 0.0f;

    // ? +++++ Iterate through the whole text to render a line +++++
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        const Character& ch = font->getChar(*c);

        float xpos = x + ch.bearing.x;
        float ypos = y - (ch.size.y - ch.bearing.y);

        float vertices[6][4] = {
            { xpos, ypos + ch.size.y, 0.0f, 0.0f },
            { xpos, ypos, 0.0f, 1.0f },
            { xpos + ch.size.x, ypos, 1.0f, 1.0f },

            { xpos, ypos + ch.size.y, 0.0f, 0.0f },
            { xpos + ch.size.x, ypos, 1.0f, 1.0f },
            { xpos + ch.size.x, ypos + ch.size.y, 1.0f, 0.0f },
        };

        // ? +++++ Render glyph texture over quad +++++
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        
        // ? +++++ Update vbo memory +++++
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // ? +++++ Render glyph quad +++++
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ? +++++ Advance cursors for next glyph +++++
        x += (ch.advance >> 6); // ! Bitshift by 6 to get value in pixels
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}