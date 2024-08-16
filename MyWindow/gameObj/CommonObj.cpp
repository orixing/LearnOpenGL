#include "CommonObj.h"

CommonObj::CommonObj(Model* model, Material* material) : GameObj(model), material(material) {}
CommonObj::~CommonObj() {}

void CommonObj::DrawInGPass(Shader* shader) {
	if (transparent) return;
	shader->setMat4("model", getModelMatrix());
	material->DrawInGPass(shader);
	Draw();
}

void CommonObj::DrawInShadowPass(Shader* shader) {
	shader->setMat4("model", getModelMatrix());
	Draw();
}