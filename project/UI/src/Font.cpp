#include "Font.hpp"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

Font::Font(FT_Face& fontFace, std::string fontPath, std::string name, unsigned int size) : fontPath(fontPath), name(name), size(size)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // ? +++++ Load character glyph +++++
        if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER))
        {
            std::cerr << "Font: Failed to load glyph " << c << '\n';
            continue;
        }

        // ? +++++ Generate texture with the glyph +++++
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                    fontFace->glyph->bitmap.width,
                    fontFace->glyph->bitmap.rows,
                    0, GL_RED, GL_UNSIGNED_BYTE,
                    fontFace->glyph->bitmap.buffer);
        
        // ? +++++ Set texture options +++++
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ? +++++ Store character for later use +++++
        Character character = {
            texture,
            glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
            glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
            fontFace->glyph->advance.x
        };

        chars.insert({c, character});
    }
}

unsigned int Font::getCharTex(char c)
{
    return chars.at(c).textureID;
}

const Character& Font::getChar(char c)
{
    return chars.at(c);
}

const char* Font::getName()
{
    return name.c_str();
}

const char* Font::getFontPath()
{
    return fontPath.c_str();
}

unsigned int Font::getSize()
{
    return size;
}