#include "BillboardingObj.h"
#include "RenderCtrl.h"
BillboardingObj::BillboardingObj(Model* model, Material* material) : GameObj(model), material(material) {}
BillboardingObj::~BillboardingObj() {}

void BillboardingObj::Draw() {
    //转向相机
    glm::mat4 modelMat = glm::mat4();
    modelMat = glm::translate(modelMat, position);
    glm::vec3 cameraDirec_xz = glm::normalize(glm::vec3(RenderCtrl::getInstance().curRenderContent->mainCamera->Direc.x, 0.0f, RenderCtrl::getInstance().curRenderContent->mainCamera->Direc.z));
    glm::vec3 n = glm::vec3(0, 0, -1);
    const glm::vec3 half = glm::normalize(cameraDirec_xz + n);
    const double w = glm::dot(n, half);
    const glm::vec3 xyz = glm::cross(n, half);
    const glm::quat p = glm::quat(w, xyz);

    glm::mat4 rotate = glm::mat4_cast(p);
    modelMat = modelMat * rotate;

    material->Draw();
    material->shader->setMat4("model", modelMat);
    material->shader->setMat4("view", RenderCtrl::getInstance().curRenderContent->mainCamera->GetViewMatrix());
    material->shader->setMat4("projection", RenderCtrl::getInstance().curRenderContent->mainCamera->GetProjectionMatrix());
	GameObj::Draw();
}