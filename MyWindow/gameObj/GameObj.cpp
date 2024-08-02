#include "GameObj.h"
GameObj::GameObj(Model* model, Material* material): model(model), material(material) {
}

GameObj::~GameObj() {}

void GameObj::Draw() {
	model->Draw();
}

void GameObj::DrawInGPass(Shader* shader) {
	shader->setMat4("model", getModelMatrix());
	material->DrawInGPass(shader);
	model->Draw();
}

glm::mat4 GameObj::getModelMatrix() {
	glm::mat4 model;
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(roll), glm::vec3(0.0, 0.0, 1.0));
	return model;
}