#include "RenderCtrl.h"
#include <glm/glm/glm.hpp>
#include <iostream>
#include "GlobalConst.h"
#include "TextureCtrl.h"

RenderCtrl& RenderCtrl::getInstance() {
    static RenderCtrl instance;
    return instance;
}



// ˽�й��캯������������
RenderCtrl::RenderCtrl() {

    GBuffer = FrameBuffer::Builder()
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gFragColor"))// - û�õ�
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gPositionDepth"))// - ��������λ��
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gNormal"))// - ����
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gAlbedoSpec"))//albedo��ɫ
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gFragPosInLight"))// �ڹ�Դ�е�λ�� ����Ҫɾ��
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gBorder"))//�Ƿ��Ǳ߽磬�ᵽextra��
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gExtra"))//������Ϣ
        .CreateRenderbuffer().Build();
}

RenderCtrl::~RenderCtrl() {
}

void RenderCtrl::Render(WindowContent* content) {
    //GPass
    RenderCtrl::getInstance().DoGPass(content);
    //--------Gpass ���
    RenderCtrl::getInstance().DoGPassBorder(content);
}

void RenderCtrl::DoGPass(WindowContent* content) {

    //todo:�����һЩ����ȫ������ʱ������Ҫ�ñ�ķ�ʽ����
    glm::vec3 pointLightPos1(6, 2.5, 4.5);
    glm::mat4 lightView = glm::lookAt(pointLightPos1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glBindFramebuffer(GL_FRAMEBUFFER, GBuffer->id);
    glViewport(0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gPassShader->use();
    gPassShader->setMat4("view", content->mainCamera->GetViewMatrix());
    gPassShader->setMat4("projection", content->mainCamera->GetProjectionMatrix());
    gPassShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glStencilMask(0xFF); // ����ģ�建��д��
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF); // ���е�Ƭ�ζ�Ӧ�ø���ģ�建��
    

    for (GameObj* oneObj : *content->allObjs) {
        oneObj->DrawInGPass(gPassShader);
    }
}

//Shader��ģ�ͷŴ�һȦ��ʹ��ģ�建��ɸѡ���Ŵ�Ĳ�����Ϊ�߽�
void RenderCtrl::DoGPassBorder(WindowContent* content) {
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_NOTEQUAL, 0xFF, 0xFF);
    glStencilMask(0x0F);
    gBorderShader->use();
    gBorderShader->setMat4("view", content->mainCamera->GetViewMatrix());
    gBorderShader->setMat4("projection", content->mainCamera->GetProjectionMatrix());

    for (GameObj* oneObj : *content->allObjs) {
        oneObj->DrawInGPass(gBorderShader);
    }

    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);
    glStencilMask(0x00);
}