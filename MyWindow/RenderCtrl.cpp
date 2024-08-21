#include "RenderCtrl.h"
#include <glm/glm/glm.hpp>
#include <iostream>
#include "GlobalConst.h"
#include "TextureCtrl.h"
#include <random> 
#include <map> 
RenderCtrl& RenderCtrl::getInstance() {
    static RenderCtrl instance;
    return instance;
}

// 私有构造函数和析构函数
RenderCtrl::RenderCtrl() {
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
}

RenderCtrl::~RenderCtrl() {
}

void RenderCtrl::Render(WindowContent* content) {
    curRenderContent = content;
    //GPass
    RenderCtrl::getInstance().DoGPass();
    //SSAOFBO
    RenderCtrl::getInstance().DoSSAOPass();
    RenderCtrl::getInstance().DoSSAOBlurPass();
    //shadow map
    RenderCtrl::getInstance().DoShadowPass();
    //PBR PASS
    //延迟渲染全部为PBR材质，只需要一个PASS
    RenderCtrl::getInstance().DoPBRRenderPass();
    //将Gbuffer深度缓冲和模板缓冲复制到fbo
    glBindFramebuffer(GL_READ_FRAMEBUFFER, curRenderContent->GBuffer->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, curRenderContent->postProcessingFBO->id); // 写入到默认帧缓冲
    glBlitFramebuffer(0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, 0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, curRenderContent->postProcessingFBO->id);


    RenderCtrl::getInstance().RenderSkybox();

    RenderCtrl::getInstance().TransparentPass();
    RenderCtrl::getInstance().RenderBorder();

    RenderCtrl::getInstance().DrawToScreen();

}

void RenderCtrl::DoGPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, curRenderContent->GBuffer->id);
    glViewport(0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gPassShader->use();
    gPassShader->setMat4("view", curRenderContent->mainCamera->GetViewMatrix());
    gPassShader->setMat4("projection", curRenderContent->mainCamera->GetProjectionMatrix());
    

    for (GameObj* oneObj : *curRenderContent->allObjs) {
        glStencilFunc(GL_ALWAYS, 0xFF, 0xFF); // 所有的片段都应该更新模板缓冲
        oneObj->renderBorder ? glStencilMask(0xFF) : glStencilMask(0x00); // 对描边的物体启用模板缓冲写入
        oneObj->DrawInGPass(gPassShader);
    }
}

void RenderCtrl::DoSSAOPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, curRenderContent->SSAOFBO->id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SSAOShader->use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->GBuffer->GetTexture("gPosition")->id);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->GBuffer->GetTexture("gNormal")->id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, TextureCtrl::getInstance().getTexture(TextureEnum::SSAONoiseTex)->id);
    for (GLuint i = 0; i < 64; ++i) {
        SSAOShader->setVec3(("samples[" + std::to_string(i) + "]").c_str(), SSAOKernel[i]);
    }
    SSAOShader->setInt("gPosition", 3);
    SSAOShader->setInt("gNormal", 4);
    SSAOShader->setInt("texNoise", 5);
    SSAOShader->setMat4("projection", curRenderContent->mainCamera->GetProjectionMatrix());
    screenObj->Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderCtrl::DoSSAOBlurPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, curRenderContent->SSAOBlurFBO->id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SSAOBlurShader->use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->SSAOFBO->GetTexture("ssaoTex")->id);
    SSAOBlurShader->setInt("ssaoInput", 3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderCtrl::DoShadowPass() {
    for (Light* light : *curRenderContent->allLights) {
        glBindFramebuffer(GL_FRAMEBUFFER, light->depthMapFBO->id);
        glViewport(0, 0, Light::ShadowMapWidth, Light::ShadowMapHeight);
        glClear(GL_DEPTH_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        shadowShader->use();
        shadowShader->setMat4("lightSpaceMatrix", light->GetLightSpaceMatrix());
        for (GameObj* oneObj : *curRenderContent->allObjs) {
            oneObj->DrawInShadowPass(shadowShader);
        }
    }
}

void RenderCtrl::DoPBRRenderPass() {
    //渲染Pass
    glBindFramebuffer(GL_FRAMEBUFFER, curRenderContent->postProcessingFBO->id);
    glViewport(0, 0, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    PBRShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->GBuffer->GetTexture("gPosition")->id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->GBuffer->GetTexture("gNormal")->id);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->GBuffer->GetTexture("gAlbedo")->id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->GBuffer->GetTexture("gExtra")->id);
    PBRShader->setInt("gPosition", 0);
    PBRShader->setInt("gNormal", 1);
    PBRShader->setInt("gAlbedo", 2);
    PBRShader->setInt("gExtra", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, curRenderContent->skyboxObj->material->IBLDiffuseLightTex->id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, curRenderContent->skyboxObj->material->IBLSpecularLightTex->id);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->skyboxObj->material->IBLSpecularBRDFTex->id);
    PBRShader->setInt("irradianceMap", 4);
    PBRShader->setInt("prefilterMap", 5);
    PBRShader->setInt("brdfLUT", 6);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, curRenderContent->SSAOBlurFBO->GetTexture("ssaoBlurTex")->id);
    PBRShader->setInt("texSSAO", 7);

    PBRShader->setInt("lightNum", curRenderContent->allLights->size());
    for (int i = 0; i < curRenderContent->allLights->size(); i++) {
        PBRShader->setVec3("lights[" + std::to_string(i) + "].position", curRenderContent->allLights->at(i)->lightPos);
        PBRShader->setVec3("lights[" + std::to_string(i) + "].intensity", curRenderContent->allLights->at(i)->color);
        PBRShader->setMat4("lights[" + std::to_string(i) + "].viewMat", curRenderContent->allLights->at(i)->GetLightView());
        PBRShader->setMat4("lights[" + std::to_string(i) + "].projMat", curRenderContent->allLights->at(i)->GetLightProjection());
        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_2D, curRenderContent->allLights->at(i)->depthMapFBO->GetTexture("depthMap")->id);
        PBRShader->setInt("lights[" + std::to_string(i) + "].shadowMap", 8+i);
    }

    PBRShader->setMat4("view", curRenderContent->mainCamera->GetViewMatrix());
    screenObj->Draw();
}

void RenderCtrl::RenderSkybox() {
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, curRenderContent->skyboxObj->material->cubeMapCache->id);
    skyboxShader->setInt("skybox", 0);
    skyboxShader->setMat4("projection", curRenderContent->mainCamera->GetProjectionMatrix());
    skyboxShader->setMat4("view", glm::mat4(glm::mat3(curRenderContent->mainCamera->GetViewMatrix())));
    curRenderContent->skyboxObj->Draw();
    glDepthFunc(GL_LESS);
}

//为透明物体排序，渲染草
void RenderCtrl::TransparentPass() {
    std::map<float, GameObj*> sorted;
    for (int i = 0; i < curRenderContent->allObjs->size(); i++) {
        GameObj* obj = curRenderContent->allObjs->at(i);
        if (!obj->transparent) continue;
        float distance = glm::length(curRenderContent->mainCamera->Position - obj->position);
        sorted[distance] = obj;
    }
    for (std::map<float, GameObj*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
        GameObj* obj = it->second;
        obj->Draw();
    }
}

//利用模板缓冲渲染描边
void RenderCtrl::RenderBorder() {

    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_NOTEQUAL, 0xFF, 0xFF);
    glStencilMask(0x0F);

    borderShader->use();
    borderShader->setMat4("view", curRenderContent->mainCamera->GetViewMatrix());
    borderShader->setMat4("projection", curRenderContent->mainCamera->GetProjectionMatrix());

    for (GameObj* oneObj : *curRenderContent->allObjs) {
        if (oneObj->renderBorder) {
            borderShader->setMat4("model", oneObj->getModelMatrix());
            oneObj->Draw();
        }
    }

    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

void RenderCtrl::DrawToScreen() {
    //将fbo绘制到输出缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (curRenderContent->useFXAA) {
        FXAAShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, curRenderContent->postProcessingFBO->GetTexture("screenTex")->id);
        FXAAShader->setInt("screenTexture", 0);
    }
    else {
        screenShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, curRenderContent->postProcessingFBO->GetTexture("screenTex")->id);
        screenShader->setInt("screenTexture", 0);
    }

    screenObj->Draw();
}
