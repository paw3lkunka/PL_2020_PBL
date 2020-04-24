#ifndef _CUBEMAP_HPP
#define _CUBEMAP_HPP

#include "ISerializable.inl"
#include "Texture.hpp"

/**
 * @brief Holds id of cubemap, pointer to raw data and create info
 */
class Cubemap : public ISerializable
{
public:
    Cubemap(TextureCreateInfo createInfo,
            unsigned char* front,
            unsigned char* left,
            unsigned char* right,
            unsigned char* back,
            unsigned char* top,
            unsigned char* bottom);
    Cubemap() = default;
    virtual ~Cubemap() = default;

    void init();
    void bind(int textureUnit);

private:
    GLuint id;
    TextureCreateInfo info;
    unsigned char* front;
    unsigned char* left;
    unsigned char* right;
    unsigned char* back;
    unsigned char* top;
    unsigned char* bottom;
};

#endif // _CUBEMAP_HPP
