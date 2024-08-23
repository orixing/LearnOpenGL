#include "TextureCtrl.h"
#include <glad/glad.h> 
#include "stb_image.h"
#include "WindowCtrl.h"
#include <random> 

static Texture* LoadSSAONoiseTex();

TextureCtrl& TextureCtrl::getInstance() {
	static TextureCtrl instance;
	return instance;
}

// ::Load2DTexture("../../MyWindow/spot/spot_texture.png");
TextureCtrl::TextureCtrl(){
    texEnum2TexLoader[TextureEnum::CowAlbedoTex] = []() {return Texture::Builder().LoadFromFile("../../MyWindow/spot/spot_texture.png").SetMipMap(true).SetName("CowAlbedoTex").Build(); };
	texEnum2TexLoader[TextureEnum::SkyboxTex_AutumnPark] = []() {return Texture::Builder().LoadFromHDRFile("../../MyWindow/skybox/autumn_park_16k.hdr").SetName("SkyboxTex").Build(); };
	texEnum2TexLoader[TextureEnum::SSAONoiseTex] = LoadSSAONoiseTex;
    texEnum2TexLoader[TextureEnum::GroundTex] = []() {return Texture::Builder().LoadFromFile("../../MyWindow/skybox/ny.png").SetMipMap(true)
        .SetWrapS(GL_MIRRORED_REPEAT).SetWrapT(GL_MIRRORED_REPEAT).SetName("groundTex").Build(); };
    texEnum2TexLoader[TextureEnum::GrassTex] = []() {return Texture::Builder().LoadFromFile("../../MyWindow/image/grass.png")
        .SetInternalFormat(GL_SRGB_ALPHA).SetFormat(GL_RGBA).SetType(GL_UNSIGNED_BYTE).SetMipMap(true).SetName("grassTex").Build(); };
}
TextureCtrl::~TextureCtrl() {}

Texture* TextureCtrl::getTexture(TextureEnum e) {
	if (texEnum2Tex.find(e) != texEnum2Tex.end()) {
		return texEnum2Tex[e];
	}
	texEnum2Tex[e] = texEnum2TexLoader[e]();
	return texEnum2Tex[e];
}

Texture* LoadSSAONoiseTex() {
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // Ëæ»ú¸¡µãÊý£¬·¶Î§0.0 - 1.0
    std::default_random_engine generator;
    std::vector<glm::vec4> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec4 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f, 0.0f);
        ssaoNoise.push_back(noise);
    }
    return Texture::Builder().SetHeight(4).SetWidth(4).SetWrapS(GL_REPEAT).SetWrapT(GL_REPEAT).SetName("ssaoNoiseTex").SetData(&ssaoNoise[0]).Build();
}