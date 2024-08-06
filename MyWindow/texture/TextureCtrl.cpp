#include "TextureCtrl.h"
#include <glad/glad.h> 
#include "stb_image.h"
#include "WindowCtrl.h"

TextureCtrl& TextureCtrl::getInstance() {
	static TextureCtrl instance;
	return instance;
}

// ::Load2DTexture("../../MyWindow/spot/spot_texture.png");
TextureCtrl::TextureCtrl(){
    texEnum2TexLoader[TextureEnum::CowAlbedoTex] = []() {return Texture::Builder().LoadFromFile("../../MyWindow/spot/spot_texture.png").SetMipMap(true).SetName("CowAlbedoTex").Build(); };
}
TextureCtrl::~TextureCtrl() {}

Texture* TextureCtrl::getTexture(TextureEnum e) {
	if (texEnum2Tex.find(e) != texEnum2Tex.end()) {
		return texEnum2Tex[e];
	}
	texEnum2Tex[e] = texEnum2TexLoader[e]();
	return texEnum2Tex[e];
}


