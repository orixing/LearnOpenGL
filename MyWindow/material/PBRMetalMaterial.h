#pragma once
#include "Material.h"
class PBRMetalMaterial : public Material
{
public:
	PBRMetalMaterial();
	~PBRMetalMaterial() override;
	void DrawInGPass(Shader* shader) override;

	Texture* albedoTex;
	float metallic;
	float roughness;
};

