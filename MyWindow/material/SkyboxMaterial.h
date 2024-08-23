#pragma once
#include "Material.h"
#include "TextureCtrl.h"
class SkyboxMaterial : public Material
{
public:
	SkyboxMaterial();
	~SkyboxMaterial() override;

	Texture* envHDRTex;
	Texture* cubeMapCache;
	Texture* IBLDiffuseLightTex;
	Texture* IBLSpecularLightTex;
	Texture* IBLSpecularBRDFTex;

	void SetTexture(TextureEnum e);
};

