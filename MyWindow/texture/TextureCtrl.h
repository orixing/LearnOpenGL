#pragma once

#include <unordered_map>
#include <string>
#include "Texture.h"
#include <vector>
#include <glm/glm/glm.hpp>
enum class TextureEnum {
	CowAlbedoTex,
	SkyboxTex_AutumnPark,
	SSAONoiseTex,
	GroundTex,
};

class TextureCtrl
{
public:

	static TextureCtrl& getInstance();
	TextureCtrl(const TextureCtrl&) = delete;
	TextureCtrl& operator=(const TextureCtrl&) = delete;

	Texture* getTexture(TextureEnum e);
private:
	TextureCtrl();
	~TextureCtrl();

	std::unordered_map<TextureEnum, Texture*> texEnum2Tex;
	std::unordered_map<TextureEnum, Texture* (*)()> texEnum2TexLoader;
};


