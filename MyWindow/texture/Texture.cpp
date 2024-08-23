#include "Texture.h"
#include "stb_image.h"
#include <iostream>

Texture::~Texture() {}
Texture::Texture(unsigned int id, const std::string& name) :id(id), name(name){}

Texture::Builder& Texture::Builder::LoadFromFile(const std::string& path) {
    int nrChannels;
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cout << "Failed to load image." << std::endl;
        return *this;
    }
    SetInternalFormat(GL_SRGB);
    SetFormat(GL_RGB);
    SetType(GL_UNSIGNED_BYTE);
    SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    SetMagFilter(GL_LINEAR);
    afterBuild.push_back([=]() {stbi_image_free(data); });
    return *this;
}

Texture::Builder& Texture::Builder::LoadFromHDRFile(const std::string& path) {
    int nrChannels;
    data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cout << "Failed to load HDR image." << std::endl;
        return *this;
    }
    SetInternalFormat(GL_RGB16F);
    SetFormat(GL_RGB);
    SetType(GL_FLOAT);
    SetMinFilter(GL_LINEAR);
    SetMagFilter(GL_LINEAR);
    afterBuild.push_back([=]() {stbi_image_free(data); });
    return *this;
}


Texture* Texture::Builder::Build() {

    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, textureMap);

    switch (target) {
    case GL_TEXTURE_2D:
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
        break;
    case GL_TEXTURE_CUBE_MAP:
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, type, data);
        }
        break;
    }
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
    switch (target) {
    case GL_TEXTURE_2D: 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        break;
    case GL_TEXTURE_CUBE_MAP:
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapR);
        break;
    }
    if (border != nullptr) {
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    }
    if(needMipMap) glGenerateMipmap(target);
    for (std::function<void()> f : afterBuild) {
        f();
    }
    return new Texture(textureMap, name);
}