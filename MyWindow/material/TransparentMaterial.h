#pragma once
#include "Material.h"
#include "TextureCtrl.h"
class TransparentMaterial : public Material
{
public:
	TransparentMaterial();
	~TransparentMaterial() override;
	Texture* texture;
};

