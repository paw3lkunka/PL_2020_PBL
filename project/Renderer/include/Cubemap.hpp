#ifndef _CUBEMAP_HPP
#define _CUBEMAP_HPP

#include "Texture.hpp"

/**
 * @brief Holds id of cubemap, pointer to raw data and create info
 */
class Cubemap : public Texture
{
    friend class SceneWriter;
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

    virtual void init();
    virtual void bind(int textureUnit) const;

    std::string frontPath;
    std::string leftPath;
    std::string rightPath;
    std::string backPath;
    std::string topPath;
    std::string bottomPath;

private:
    unsigned char* front;
    unsigned char* left;
    unsigned char* right;
    unsigned char* back;
    unsigned char* top;
    unsigned char* bottom;
};

#endif // _CUBEMAP_HPP
