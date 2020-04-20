#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <glad/glad.h>

struct TextureCreateInfo
{
    bool generateMipmaps;
    int width, height;
    GLenum format, wrapMode, minFilter, magFilter;
};

/**
 * @brief Holds GL id of bound texture, pointer to raw data and texture type
 * 
 */
class Texture
{
public:
    //TODO documentation
    Texture(unsigned char* data, TextureCreateInfo createInfo) : data(data), info(createInfo) { init(); }
    Texture() = default;
    ~Texture() = default;

    //TODO documentation
    void init();
    //TODO documentation
    void bind(int textureUnit);

private:
    GLuint id;
    TextureCreateInfo info;
    unsigned char* data;
};

#endif // _TEXTURE_HPP
