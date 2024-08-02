#include "GPassCtrl.h"
#include <glm/glm/glm.hpp>
#include <iostream>
#include "GlobalConst.h"
#include "CameraCtrl.h"

GPassCtrl& GPassCtrl::getInstance() {
    static GPassCtrl instance;
    return instance;
}

// 私有构造函数和析构函数
GPassCtrl::GPassCtrl() {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // - 位置颜色缓冲
    glGenTextures(1, &gFragColor);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, gFragColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFragColor, 0);

    // - 位置颜色缓冲
    glGenTextures(1, &gPositionDepth);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPositionDepth, 0);

    // - 法线颜色缓冲
    glGenTextures(1, &gNormal);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

    // - 颜色 + 镜面颜色缓冲
    glGenTextures(1, &gAlbedoSpec);
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAlbedoSpec, 0);

    glGenTextures(1, &gFragPosInLight);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, gFragPosInLight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gFragPosInLight, 0);

    //
    glGenTextures(1, &gBorder);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, gBorder);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gBorder, 0);

    glGenTextures(1, &gExtra);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, gExtra);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gExtra, 0);


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

GPassCtrl::~GPassCtrl() {
}

void GPassCtrl::DoGPass(std::vector<GameObj*>* allObjs) {

    //todo:这里和一些参数全部是临时传来，要用别的方式访问
    glm::vec3 pointLightPos1(6, 2.5, 4.5);
    glm::mat4 lightView = glm::lookAt(pointLightPos1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glViewport(0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gPassShader->use();
    gPassShader->setMat4("view", CameraCtrl::getInstance().mainCamera->GetViewMatrix());
    gPassShader->setMat4("projection", CameraCtrl::getInstance().mainCamera->GetProjectionMatrix());
    gPassShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glStencilMask(0xFF); // 启用模板缓冲写入
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF); // 所有的片段都应该更新模板缓冲
    
    for (GameObj* oneObj : *allObjs) {
        oneObj->DrawInGPass(gPassShader);
    }
}