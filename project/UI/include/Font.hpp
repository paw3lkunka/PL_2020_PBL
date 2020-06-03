#ifndef _FONT_HPP
#define _FONT_HPP

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ISerializable.hpp"

struct Character
{
    unsigned int textureID; // ID of generated glyph texture
    glm::ivec2 size; // Size of glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    unsigned int advance; // Offset to advance to next glyph
};

class Font : public ISerializable
{
public:
    /**
     * @brief Construct a new Font object
     * 
     * @param fontFace FreeType font face object
     */
    Font(FT_Face& fontFace, std::string fontPath, std::string name, unsigned int size);
    virtual ~Font() = default;

    /**
     * @brief Retrieve id of specific character texture
     * 
     * @param c charater to be retrieved
     * @return unsigned int texture id
     */
    unsigned int getCharTex(char c);

    /**
     * @brief Get name of the material
     * 
     * @return const char* name of the material
     */
    const char* getName();

    /**
     * @brief Get the Font path
     * 
     * @return const char* path to font
     */
    const char* getFontPath();

    /**
     * @brief Get the font size object
     * 
     * @return unsigned int font size
     */
    unsigned int getSize();

private:
    std::string fontPath;
    std::string name;
    unsigned int size;
    std::unordered_map<char, Character> chars;
};

#endif // _FONT_HPP
