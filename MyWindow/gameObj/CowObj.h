#pragma once
#include "GameObj.h"
class CowObj : public GameObj
{
public:
	CowObj(Model* model, Material* material);
	~CowObj() override;
};

