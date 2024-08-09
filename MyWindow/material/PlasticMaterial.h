#pragma once
#include "Material.h"
class PlasticMaterial : public Material
{
public:
	PlasticMaterial();

	~PlasticMaterial() override;
	void DrawInGPass(Shader* shader) override;

	Texture* albedoTex;

};

