#pragma once
#include "GameObj.h"
#include "SkyboxMaterial.h"
class SkyboxObj : public GameObj
{
public:
	SkyboxObj();
	~SkyboxObj();

	SkyboxMaterial* material;

	void PrecomputeIBL();
};

