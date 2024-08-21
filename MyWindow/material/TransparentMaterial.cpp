#include "TransparentMaterial.h"
TransparentMaterial::TransparentMaterial() : Material(MaterialEnum::Transparent) {}

TransparentMaterial::~TransparentMaterial() {}

void TransparentMaterial::Draw() {
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	shader->setInt("grassTex", 0);
}

