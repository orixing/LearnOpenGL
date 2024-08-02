#pragma once

#include <unordered_map>
#include <string>
#include "Texture.h"
enum class TextureEnum {
	CowAlbedoTex,
};

class TextureCtrl
{
public:

	static TextureCtrl& getInstance();
	TextureCtrl(const TextureCtrl&) = delete;
	TextureCtrl& operator=(const TextureCtrl&) = delete;

	Texture* getTexture(TextureEnum e);

	static Texture* Load2DTexture(const std::string& path);
	static Texture* CreateEmpty2DTexture();

private:
	TextureCtrl();
	~TextureCtrl();

	std::unordered_map<TextureEnum, Texture*> texEnum2Tex;
	std::unordered_map<TextureEnum, Texture* (*)()> texEnum2TexLoader;
};


