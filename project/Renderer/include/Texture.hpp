#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <glad/glad.h>

struct TextureCreateInfo
{
    bool generateMipmaps;
    int nrChannels, width, height;
    GLenum format, wrapMode, minFilter, magFilter;
};

/**
 * @brief Holds GL id of bound texture, pointer to raw data and texture type
 * 
 */
class Texture
{
public:
    Texture(unsigned char* data, TextureCreateInfo createInfo) : data(data), info(createInfo) {}
    Texture() = default;
    ~Texture() = default;

    void init();
    void bind(int textureUnit, const char* name);

private:
    GLuint id;
    TextureCreateInfo info;
    unsigned char* data;
};

#endif // _TEXTURE_HPP
