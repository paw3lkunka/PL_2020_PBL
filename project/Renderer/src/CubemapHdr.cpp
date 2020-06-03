#include "CubemapHdr.hpp"

CubemapHdr::CubemapHdr(TextureCreateInfo createInfo,
            float* front,
            float* left,
            float* right,
            float* back,
            float* top,
            float* bottom) 
    : Texture(nullptr, createInfo, ""), 
    front(front),
    left(left),
    right(right),
    back(back),
    top(top),
    bottom(bottom) 
{
    init();
}

void CubemapHdr::init()
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, info.magFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, info.wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, info.wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, info.wrapMode);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB16F, info.width, info.height, 0, GL_RGB, GL_FLOAT, right);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB16F, info.width, info.height, 0, GL_RGB, GL_FLOAT, left);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB16F, info.width, info.height, 0, GL_RGB, GL_FLOAT, top);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB16F, info.width, info.height, 0, GL_RGB, GL_FLOAT, bottom);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB16F, info.width, info.height, 0, GL_RGB, GL_FLOAT, back);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB16F, info.width, info.height, 0, GL_RGB, GL_FLOAT, front);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubemapHdr::bind(int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}