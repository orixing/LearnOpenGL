#include "Material.h"

Material::Material(MaterialEnum e):materialEnum(e){};

Material::~Material() {}

void Material::DrawInGPass(Shader* shader) {
	shader->setFloat("materialType", (float)materialEnum);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoTex->id);
	shader->setInt("albedoTex", 0);
}