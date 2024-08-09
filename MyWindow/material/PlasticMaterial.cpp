#include "PlasticMaterial.h"
PlasticMaterial::PlasticMaterial() : Material(MaterialEnum::Plastic){}

PlasticMaterial::~PlasticMaterial() {}

void PlasticMaterial::DrawInGPass(Shader* shader) {
	shader->setFloat("materialType", (float)materialEnum);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoTex->id);
	shader->setInt("albedoTex", 0);
}
