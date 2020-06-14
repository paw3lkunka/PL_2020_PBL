#ifndef _CUBEMAPHDR_HPP
#define _CUBEMAPHDR_HPP

#include "Texture.hpp"

#include <string>

class CubemapHdr : public Texture
{
    friend class SceneWriter;
public:
    CubemapHdr(TextureCreateInfo createInfo,
            float* front,
            float* left,
            float* right,
            float* back,
            float* top,
            float* bottom);
    CubemapHdr(unsigned int externalID);
    CubemapHdr() = default;
    virtual ~CubemapHdr() = default;

    /**
     * @brief Initializes hdr cubemap with data and options.
     */
    virtual void init();
    /**
     * @brief Bind cubemap to provided texture unit
     * 
     * @param textureUnit texture unit id
     */
    virtual void bind(int textureUnit) const;

    std::string frontPath;
    std::string leftPath;
    std::string rightPath;
    std::string backPath;
    std::string topPath;
    std::string bottomPath;

private:
    float* front;
    float* left;
    float* right;
    float* back;
    float* top;
    float* bottom;
};

#endif // _CUBEMAPHDR_HPP
