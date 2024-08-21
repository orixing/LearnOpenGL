#pragma once
#include "GameObj.h"
class BillboardingObj : public GameObj
{
public:
	Material* material;

	BillboardingObj(Model* model, Material* material);
	~BillboardingObj();

	void Draw() override;
};

