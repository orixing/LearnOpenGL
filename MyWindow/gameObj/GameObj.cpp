#include "GameObj.h"
#include "GuidCtrl.h"
GameObj::GameObj(Model* model): model(model){
    guid = GuidCtrl::getInstance().GenGuid();
}

GameObj::~GameObj() {}

glm::mat4 GameObj::getModelMatrix() {
	glm::mat4 model;
	model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));
	model = glm::rotate(model, glm::radians(roll), glm::vec3(0.0, 0.0, 1.0));
    model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0, 0.0, 0.0));
	return model;
}

glm::quat GameObj::getRotationQuad() {
    // ������ÿ�������ת��Ԫ��
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qRoll = glm::angleAxis(glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));

    // ��˳��ϳ���Ԫ�����������ת˳���� XYZ��
    return qRoll * qYaw * qPitch;
}

void GameObj::setRotationByQuad(glm::quat q) {
    glm::vec3 eulerAngles = glm::eulerAngles(q);
    pitch = glm::degrees(eulerAngles.x);
    yaw = glm::degrees(eulerAngles.y);
    roll = glm::degrees(eulerAngles.z);

}

void GameObj::DrawInGPass(Shader* shader){}//todo������������������,shader���ڲ������ʱ�俴�����������ʵ��
void GameObj::DrawInShadowPass(Shader* shader) {}
void GameObj::Draw() {
	model->Draw();
}

Mesh* GameObj::GetMesh() {
    return &model->meshes[0];
}

void GameObj::UpdatePosition(glm::vec3 deltaX) {
    position += deltaX;
}

void  GameObj::UpdateRotation(glm::vec3 dw) {
    glm::quat q = getRotationQuad();
    q = q + glm::quat(0.0, dw.x, dw.y, dw.z) * q;
    setRotationByQuad(q);
}

glm::vec3 GameObj::GetPosition() {
    return position;
}
glm::quat GameObj::GetRotation() {
    return getRotationQuad();
}
glm::vec3 GameObj::GetRadiusVector(glm::vec3 localPos) {
    return glm::vec3(scaleX, scaleY, scaleZ) * getRotationQuad() * localPos;
}

glm::vec3 GameObj::GetScale() {
    return glm::vec3(scaleX, scaleY, scaleZ);
}

long GameObj::GetGuid() {
    return guid;
}