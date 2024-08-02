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
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // - 没用到
    gFragColor = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFragColor->id, 0);

    
    // - 世界坐标位置
    gPositionDepth = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPositionDepth->id, 0);

    // - 法线
    gNormal = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal->id, 0);

    // - albedo颜色
    gAlbedoSpec = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAlbedoSpec->id, 0);

    // 在光源中的位置 将来要删掉
    gFragPosInLight = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gFragPosInLight->id, 0);

    //是否是边界，搬到extra里
    gBorder = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gBorder->id, 0);

    //材质信息
    gExtra = TextureCtrl::CreateEmpty2DTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gExtra->id, 0);

    // - 告诉OpenGL我们将要使用(帧缓冲的)哪种颜色附件来进行渲染
    GLuint attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3 ,GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 ,GL_COLOR_ATTACHMENT6 };
    glDrawBuffers(7, attachments);

    // 之后同样添加渲染缓冲对象(Render Buffer Object)为深度缓冲(Depth Buffer)，并检查完整性
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Gbuffer not complete!" << std::endl;
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

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
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