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

    GBuffer = new FrameBuffer();

    GBuffer->BindTexture(Texture::Builder().SetName("gFragColor").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);// - 没用到
    GBuffer->BindTexture(Texture::Builder().SetName("gPositionDepth").Build(), GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D);// - 世界坐标位置
    GBuffer->BindTexture(Texture::Builder().SetName("gNormal").Build(), GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D);// - 法线
    GBuffer->BindTexture(Texture::Builder().SetName("gAlbedoSpec").Build(), GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D);//albedo颜色
    GBuffer->BindTexture(Texture::Builder().SetName("gFragPosInLight").Build(), GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D);// 在光源中的位置 将来要删掉
    GBuffer->BindTexture(Texture::Builder().SetName("gBorder").Build(), GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D);//是否是边界，搬到extra里
    GBuffer->BindTexture(Texture::Builder().SetName("gExtra").Build(), GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D);//材质信息
    GBuffer->AddRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    GBuffer->SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 });

    //---------------预准备SSAO数据
    PrepareSSAOData();

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
    //PBR PASS
    //延迟渲染全部为PBR材质，只需要一个PASS
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
    glBindTexture(GL_TEXTURE_2D, TextureCtrl::getInstance().getTexture(TextureEnum::SSAONoiseTex)->id);
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

void RenderCtrl::DoPBRRenderPass(WindowContent* content) {
    //渲染Pass
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glViewport(0, 0, screenWidth, screenHeight);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //PBRShader.use();
    //glBindVertexArray(screenVAO);

    //glActiveTexture(GL_TEXTURE6);
    //glBindTexture(GL_TEXTURE_2D, content->allLights->at(0)->depthMapFBO->GetTexture("depthMap")->id);

    //glActiveTexture(GL_TEXTURE13);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gAlbedoSpec")->id);

    //glActiveTexture(GL_TEXTURE10);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gFragColor")->id);

    //glActiveTexture(GL_TEXTURE9);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gExtra")->id);
    //glActiveTexture(GL_TEXTURE11);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gPositionDepth")->id);
    //glActiveTexture(GL_TEXTURE12);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gNormal")->id);
    //glActiveTexture(GL_TEXTURE14);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gFragPosInLight")->id);

    //PBRShader.setInt("shadowMap", 6);
    //PBRShader.setInt("gExtra", 9);
    ////PBRShader.setInt("gFragColor", 10);
    //PBRShader.setInt("gPosition", 11);
    //PBRShader.setInt("gNormal", 12);
    //PBRShader.setInt("gAlbedo", 13);
    //PBRShader.setInt("gFragPosLightSpace", 14);
    //glActiveTexture(GL_TEXTURE7);
    //glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().SSAOBlurFBO->GetTexture("ssaoBlurTex")->id);//todo:临时处理，这里是错误的
    //PBRShader.setInt("texSSAO", 7);
    //PBRShader.setMat4("view", content->mainCamera->GetViewMatrix());
    //PBRShader.setVec3("lightPos", content->allLights->at(0)->lightPos);
    //PBRShader.setVec3("lightColor", glm::vec3(50.0f, 50.0f, 50.0f));
    //glActiveTexture(GL_TEXTURE15);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->IBLDiffuseLightTex->id);
    //PBRShader.setInt("irradianceMap", 15);
    //glActiveTexture(GL_TEXTURE4);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->IBLSpecularLightTex->id);
    //PBRShader.setInt("prefilterMap", 4);
    //glActiveTexture(GL_TEXTURE5);
    //glBindTexture(GL_TEXTURE_2D, content->skyboxObj->material->IBLSpecularBRDFTex->id);
    //PBRShader.setInt("brdfLUT", 5);
    //PBRShader.setMat4("lightInverseProj", glm::inverse(content->allLights->at(0)->GetLightProjection()));
    //glDrawArrays(GL_TRIANGLES, 0, 6);


}


void RenderCtrl::PrepareSSAOData() {
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

    TextureCtrl::getInstance().getTexture(TextureEnum::SSAONoiseTex);

    SSAOFBO = new FrameBuffer();
    SSAOFBO->BindTexture(Texture::Builder().SetName("ssaoTex").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

    SSAOBlurFBO = new FrameBuffer();
    SSAOBlurFBO->BindTexture(Texture::Builder().SetName("ssaoBlurTex").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);
}
