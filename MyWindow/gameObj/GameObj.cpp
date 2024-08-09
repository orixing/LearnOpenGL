#include "GameObj.h"
GameObj::GameObj(Model* model): model(model){
}

GameObj::~GameObj() {}

glm::mat4 GameObj::getModelMatrix() {
	glm::mat4 model;
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(roll), glm::vec3(0.0, 0.0, 1.0));
	return model;
}

void GameObj::DrawInGPass(Shader* shader){}
void GameObj::DrawInShadowPass(Shader* shader) {}
void GameObj::Draw() {
	model->Draw();
}