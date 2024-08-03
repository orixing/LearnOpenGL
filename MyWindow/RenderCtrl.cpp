#include "RenderCtrl.h"
#include <glm/glm/glm.hpp>
#include <iostream>
#include "GlobalConst.h"
#include "TextureCtrl.h"

RenderCtrl& RenderCtrl::getInstance() {
    static RenderCtrl instance;
    return instance;
}



// 私有构造函数和析构函数
RenderCtrl::RenderCtrl() {

    GBuffer = FrameBuffer::Builder()
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gFragColor"))// - 没用到
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gPositionDepth"))// - 世界坐标位置
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gNormal"))// - 法线
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gAlbedoSpec"))//albedo颜色
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gFragPosInLight"))// 在光源中的位置 将来要删掉
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gBorder"))//是否是边界，搬到extra里
        .BindTexture(TextureCtrl::CreateEmpty2DTexture("gExtra"))//材质信息
        .CreateRenderbuffer().Build();
}

RenderCtrl::~RenderCtrl() {
}

void RenderCtrl::Render(WindowContent* content) {
    //GPass
    RenderCtrl::getInstance().DoGPass(content);
    //--------Gpass 描边
    RenderCtrl::getInstance().DoGPassBorder(content);
}

void RenderCtrl::DoGPass(WindowContent* content) {

    //todo:这里和一些参数全部是临时传来，要用别的方式访问
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
    glStencilMask(0xFF); // 启用模板缓冲写入
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF); // 所有的片段都应该更新模板缓冲
    

    for (GameObj* oneObj : *content->allObjs) {
        oneObj->DrawInGPass(gPassShader);
    }
}

//Shader将模型放大一圈，使用模板缓冲筛选出放大的部分作为边界
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