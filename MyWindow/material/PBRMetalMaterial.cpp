#include "PBRMetalMaterial.h"

PBRMetalMaterial::PBRMetalMaterial() : Material(MaterialEnum::PBRMetal){}

PBRMetalMaterial::~PBRMetalMaterial() {}

void PBRMetalMaterial::DrawInGPass(Shader* shader) {
	Material::DrawInGPass(shader);
	shader->setFloat("metallic", metallic);
	shader->setFloat("roughness", roughness);
}

