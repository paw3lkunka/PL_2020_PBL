#include "Cubemap.hpp"

Cubemap::Cubemap(TextureCreateInfo createInfo,
            unsigned char* front,
            unsigned char* left,
            unsigned char* right,
            unsigned char* back,
            unsigned char* top,
            unsigned char* bottom) :
            info(createInfo),
            front(front),
            left(left),
            right(right),
            back(back),
            top(top),
            bottom(bottom) 
{
    init();
}

void Cubemap::init()
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, front);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, left);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, right);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, back);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, top);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, bottom);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, info.magFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, info.wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, info.wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, info.wrapMode);
}

void Cubemap::bind(int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}