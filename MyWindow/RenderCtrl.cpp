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

// ˽�й��캯������������
RenderCtrl::RenderCtrl() {

    GBuffer = new FrameBuffer();

    GBuffer->BindTexture(Texture::Builder().SetName("gPosition").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);// - ��������λ��
    GBuffer->BindTexture(Texture::Builder().SetName("gNormal").Build(), GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D);// - ����
    GBuffer->BindTexture(Texture::Builder().SetName("gAlbedo").Build(), GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D);//albedo��ɫ
    GBuffer->BindTexture(Texture::Builder().SetName("gExtra").Build(), GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D);//������Ϣ
    GBuffer->AddRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    GBuffer->SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 });

    //---------------Ԥ׼��SSAO����
    PrepareSSAOData();

    postProcessingFBO = new FrameBuffer();
    postProcessingFBO->BindTexture(Texture::Builder().SetName("screenTex").SetInternalFormat(GL_SRGB).SetFormat(GL_RGB).SetType(GL_UNSIGNED_BYTE).Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);
    postProcessingFBO->AddRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);

    //todo:Screen��VAO��������ô��ʼ������
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
    //SSAOFBO
    RenderCtrl::getInstance().DoSSAOPass(content);
    RenderCtrl::getInstance().DoSSAOBlurPass(content);
    //shadow map
    RenderCtrl::getInstance().DoShadowPass(content);
    //PBR PASS
    //�ӳ���Ⱦȫ��ΪPBR���ʣ�ֻ��Ҫһ��PASS
    RenderCtrl::getInstance().DoPBRRenderPass(content);
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
    

    for (GameObj* oneObj : *content->allObjs) {

        glStencilFunc(GL_ALWAYS, 0xFF, 0xFF); // ���е�Ƭ�ζ�Ӧ�ø���ģ�建��
        oneObj->renderBorder ? glStencilMask(0xFF) : glStencilMask(0x00); // ����ߵ���������ģ�建��д��
        oneObj->DrawInGPass(gPassShader);
    }
}

void RenderCtrl::DoSSAOPass(WindowContent* content) {
    glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO->id);
    glBindVertexArray(screenVAO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SSAOShader->use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gPosition")->id);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gNormal")->id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, TextureCtrl::getInstance().getTexture(TextureEnum::SSAONoiseTex)->id);
    for (GLuint i = 0; i < 64; ++i) {
        SSAOShader->setVec3(("samples[" + std::to_string(i) + "]").c_str(), SSAOKernel[i]);
    }
    SSAOShader->setInt("gPosition", 3);
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

void RenderCtrl::DoPBRRenderPass(WindowContent* content) {
    //��ȾPass
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO->id);
    glViewport(0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    PBRShader->use();
    glBindVertexArray(screenVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gPosition")->id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gNormal")->id);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gAlbedo")->id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gExtra")->id);
    PBRShader->setInt("gPosition", 0);
    PBRShader->setInt("gNormal", 1);
    PBRShader->setInt("gAlbedo", 2);
    PBRShader->setInt("gExtra", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->IBLDiffuseLightTex->id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->IBLSpecularLightTex->id);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, content->skyboxObj->material->IBLSpecularBRDFTex->id);
    PBRShader->setInt("irradianceMap", 4);
    PBRShader->setInt("prefilterMap", 5);
    PBRShader->setInt("brdfLUT", 6);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().SSAOBlurFBO->GetTexture("ssaoBlurTex")->id);//todo:��ʱ���������Ǵ����
    PBRShader->setInt("texSSAO", 7);

    PBRShader->setInt("lightNum", content->allLights->size());
    for (int i = 0; i < content->allLights->size(); i++) {
        PBRShader->setVec3("lights[" + std::to_string(i) + "].position", content->allLights->at(i)->lightPos);
        PBRShader->setVec3("lights[" + std::to_string(i) + "].intensity", content->allLights->at(i)->color);
        PBRShader->setMat4("lights[" + std::to_string(i) + "].viewMat", content->allLights->at(i)->GetLightView());
        PBRShader->setMat4("lights[" + std::to_string(i) + "].projMat", content->allLights->at(i)->GetLightProjection());
        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_2D, content->allLights->at(i)->depthMapFBO->GetTexture("depthMap")->id);
        PBRShader->setInt("lights[" + std::to_string(i) + "].shadowMap", 8+i);
    }

    PBRShader->setMat4("view", content->mainCamera->GetViewMatrix());
    glDrawArrays(GL_TRIANGLES, 0, 6);


}


void RenderCtrl::PrepareSSAOData() {
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // �������������Χ0.0 - 1.0
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

    TextureCtrl::getInstance().getTexture(TextureEnum::SSAONoiseTex);

    SSAOFBO = new FrameBuffer();
    SSAOFBO->BindTexture(Texture::Builder().SetName("ssaoTex").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

    SSAOBlurFBO = new FrameBuffer();
    SSAOBlurFBO->BindTexture(Texture::Builder().SetName("ssaoBlurTex").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);
}
