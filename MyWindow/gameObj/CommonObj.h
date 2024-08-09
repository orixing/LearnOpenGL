#pragma once
#include "GameObj.h"
class CommonObj : public GameObj
{
public:
	Material* material;

	CommonObj(Model* model, Material* material);
	~CommonObj();

	void DrawInGPass(Shader* shader) override;
	void DrawInShadowPass(Shader* shader) override;
};

