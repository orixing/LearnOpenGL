#pragma once
#include "GameObj.h"
class CommonObj : public GameObj
{
public:
	Material* material;

	CommonObj(Model* model, Material* material);
	~CommonObj();

	void DrawInGPass(Shader* shader) override;//todo：不该有这两个方法
	void DrawInShadowPass(Shader* shader) override;
};

