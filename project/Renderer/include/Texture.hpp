#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <glad/glad.h>

#include "ISerializable.hpp"
#include "IFileSystem.inl"

class SceneWriter;

/**
 * @brief Texture create info, all values are required
 */
struct TextureCreateInfo
{
    bool generateMipmaps;
    int width, height;
    GLenum format, wrapMode, minFilter, magFilter;
    GLenum type = GL_UNSIGNED_BYTE;
};

/**
 * @brief Holds GL id of bound texture, pointer to raw data and texture info
 */
class Texture : public ISerializable, public IFileSystem
{
    friend class SceneWriter;
public:
    /**
     * @brief Construct a new Texture object
     * 
     * @param data byte pointer to raw texture data
     * @param createInfo create options of texture
     * @param filePath path to file from texture is loaded
     */
    Texture(unsigned char* data, TextureCreateInfo createInfo, std::string filePath);
    Texture(unsigned int externalID);
    Texture() = default;
    virtual ~Texture() = default;

    /**
     * @brief Initializes texture with data and options from createInfo
     */
    virtual void init();
    /**
     * @brief Bind texture to provided texture unit
     * 
     * @param textureUnit Texture unit to set
     */
    virtual void bind(int textureUnit) const;
    /**
     * @brief Get the the texture ID
     * 
     * @return unsigned int texture's ID
     */
    unsigned int getId();

protected:
    unsigned int id;
    TextureCreateInfo info;

private:
    unsigned char* data;
};

#endif // _TEXTURE_HPP
