#include "Texture.h"
#include "stb_image.h"

Texture::~Texture() {}
Texture::Texture(unsigned int id, const std::string& name) :id(id), name(name){}

Texture::Builder& Texture::Builder::LoadFromFile(const std::string& path) {
    int nrChannels;
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    SetInternalFormat(GL_SRGB);
    SetFormat(GL_RGB);
    SetType(GL_UNSIGNED_BYTE);
    SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    SetMagFilter(GL_LINEAR);
    afterBuild.push_back([=]() {stbi_image_free(data); });
    return *this;
}


Texture* Texture::Builder::Build() {

    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, textureMap);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
    switch (target) {
    case GL_TEXTURE_2D: 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
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