#include "RenderCtrl.h"
#include <glm/glm/glm.hpp>
#include <iostream>
#include "GlobalConst.h"
#include "TextureCtrl.h"
#include <random> 
RenderCtrl& RenderCtrl::getInstance() {
    static RenderCtrl instance;
    return instance;
}

// 私有构造函数和析构函数
RenderCtrl::RenderCtrl() {

    GBuffer = FrameBuffer::Builder()
        .BindColorTexture(Texture::Builder().SetName("gFragColor").Build())// - 没用到
        .BindColorTexture(Texture::Builder().SetName("gPositionDepth").Build())// - 世界坐标位置
        .BindColorTexture(Texture::Builder().SetName("gNormal").Build())// - 法线
        .BindColorTexture(Texture::Builder().SetName("gAlbedoSpec").Build())//albedo颜色
        .BindColorTexture(Texture::Builder().SetName("gFragPosInLight").Build())// 在光源中的位置 将来要删掉
        .BindColorTexture(Texture::Builder().SetName("gBorder").Build())//是否是边界，搬到extra里
        .BindColorTexture(Texture::Builder().SetName("gExtra").Build())//材质信息
        .CreateRenderbuffer().Build();

    //---------------预准备SSAO数据
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // 随机浮点数，范围0.0 - 1.0
    std::default_random_engine generator;

    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        scale = 0.1 + 0.9 * scale * scale;
        sample *= scale;
        SSAOKernel.push_back(sample);
    }

    std::vector<glm::vec4> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec4 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f, 0.0f);
        ssaoNoise.push_back(noise);
    }

    SSAONoiseTex = Texture::Builder().SetHeight(4).SetWidth(4).SetWrapS(GL_REPEAT).SetWrapT(GL_REPEAT).SetName("ssaoNoiseTex").SetData(&ssaoNoise[0]).Build();

    SSAOFBO = FrameBuffer::Builder()
        .BindColorTexture(Texture::Builder().SetName("ssaoTex").Build()).Build();

    SSAOBlurFBO = FrameBuffer::Builder()
        .BindColorTexture(Texture::Builder().SetName("ssaoBlurTex").Build()).Build();

    //todo:Screen的VAO，看看怎么初始化合适
    float screenVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &screenVAO);
    unsigned int screenVBO;
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

RenderCtrl::~RenderCtrl() {
}

void RenderCtrl::Render(WindowContent* content) {
    //GPass
    RenderCtrl::getInstance().DoGPass(content);
    //--------Gpass 描边
    RenderCtrl::getInstance().DoGBorderPass(content);
    //SSAOFBO
    RenderCtrl::getInstance().DoSSAOPass(content);
    RenderCtrl::getInstance().DoSSAOBlurPass(content);
    //shadow map
    RenderCtrl::getInstance().DoShadowPass(content);
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
void RenderCtrl::DoGBorderPass(WindowContent* content) {
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


void RenderCtrl::DoSSAOPass(WindowContent* content) {
    glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO->id);
    glBindVertexArray(screenVAO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SSAOShader->use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gPositionDepth")->id);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gNormal")->id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, SSAONoiseTex->id);
    for (GLuint i = 0; i < 64; ++i) {
        SSAOShader->setVec3(("samples[" + std::to_string(i) + "]").c_str(), SSAOKernel[i]);
    }
    SSAOShader->setInt("gPositionDepth", 3);
    SSAOShader->setInt("gNormal", 4);
    SSAOShader->setInt("texNoise", 5);
    SSAOShader->setMat4("projection", content->mainCamera->GetProjectionMatrix());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderCtrl::DoSSAOBlurPass(WindowContent* content) {
    glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFBO->id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SSAOBlurShader->use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, SSAOFBO->GetTexture("ssaoTex")->id);
    SSAOBlurShader->setInt("ssaoInput", 3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderCtrl::DoShadowPass(WindowContent* content) {

    for (Light* light : *content->allLights) {
        glBindFramebuffer(GL_FRAMEBUFFER, light->depthMapFBO->id);
        glViewport(0, 0, Light::ShadowMapWidth, Light::ShadowMapHeight);
        glClear(GL_DEPTH_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        shadowShader->use();
        shadowShader->setMat4("lightSpaceMatrix", light->GetLightSpaceMatrix());
        for (GameObj* oneObj : *content->allObjs) {
            oneObj->DrawInShadowPass(shadowShader);
        }
    }
}