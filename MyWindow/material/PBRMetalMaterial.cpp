#include "PBRMetalMaterial.h"

PBRMetalMaterial::PBRMetalMaterial() : Material(MaterialEnum::PBRMetal){}

PBRMetalMaterial::~PBRMetalMaterial() {}

void PBRMetalMaterial::DrawInGPass(Shader* shader) {
	shader->setFloat("materialType", (float)materialEnum);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoTex->id);
	shader->setInt("albedoTex", 0);
	shader->setFloat("metallic", metallic);
	shader->setFloat("roughness", roughness);
}

