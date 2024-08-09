#pragma once
#include "Shader.h"
#include "MaterialEnum.h"
#include "Texture.h"
class Material
{
public:
	Material(MaterialEnum e);
	virtual ~Material() = 0;
	MaterialEnum materialEnum;
	virtual void DrawInGPass(Shader* shader);
};
