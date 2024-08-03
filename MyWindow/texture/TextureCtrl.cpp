#include "TextureCtrl.h"
#include <glad/glad.h> 
#include "stb_image.h"
#include "WindowCtrl.h"

TextureCtrl& TextureCtrl::getInstance() {
	static TextureCtrl instance;
	return instance;
}

TextureCtrl::TextureCtrl(){
	texEnum2TexLoader[TextureEnum::CowAlbedoTex] = [](){return TextureCtrl::Load2DTexture("../../MyWindow/spot/spot_texture.png");};
}
TextureCtrl::~TextureCtrl() {}

Texture* TextureCtrl::getTexture(TextureEnum e) {
	if (texEnum2Tex.find(e) != texEnum2Tex.end()) {
		return texEnum2Tex[e];
	}
	texEnum2Tex[e] = texEnum2TexLoader[e]();
	return texEnum2Tex[e];
}

Texture* TextureCtrl::Load2DTexture(const std::string& path) {
    int width, height, nrChannels;
    unsigned char* data;
    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return new Texture(textureMap, path);
}

Texture* TextureCtrl::CreateEmpty2DTexture(const std::string& name) {
    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowCtrl::ScreenWidth, WindowCtrl::ScreenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return new Texture(textureMap, name);
}
