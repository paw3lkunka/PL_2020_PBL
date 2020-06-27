#include "Texture.hpp"
#include <iostream>

Texture::Texture(unsigned char* data, TextureCreateInfo createInfo, std::string filePath, bool initTex)
    : IFileSystem(filePath), data(data), info(createInfo)
{
    if (initTex)
    {
        init(); 
    }
}

Texture::Texture(unsigned int externalID) : IFileSystem(""), data(nullptr)
{
    id = externalID;
}

void Texture::init()
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, info.internalFormat, info.width, info.height, 0, info.format, info.type, data);
    if (info.generateMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, info.wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, info.wrapMode);
    if (info.wrapMode == GL_CLAMP_TO_BORDER)
    {
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info.magFilter);
}

void Texture::bind(int textureUnit) const
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int Texture::getId()
{
    return id;
}