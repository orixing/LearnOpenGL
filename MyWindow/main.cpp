#include "Camera.h"
#include "CommonObj.h"
#include "GameObj.h"
#include "Model.h"
#include "PBRMetalMaterial.h"
#include "PlasticMaterial.h"
#include "RenderCtrl.h"
#include "Shader.h"
#include "stb_image.h"
#include "TextureCtrl.h"
#include "WindowCtrl.h"
#include "SkyboxMaterial.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map> 
#include <random> 
const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

glm::vec3 grassTranslate[] = {
    glm::vec3(0.5f,-0.5,2),glm::vec3(-1,-0.5,2),glm::vec3(0,-0.5,3)
};

int main(void)
{

    glfwWindowHint(GLFW_SAMPLES, 4);//todo:这个msaa选项只影响下一个创建的窗口
    GLFWwindow* window = WindowCtrl::getInstance().NewWindow();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    stbi_set_flip_vertically_on_load(true);

    Shader lightShader("../../MyWindow/rawShaders/vertexShader.vs", "../../MyWindow/rawShaders/lightFregmentShader.fs");
    Shader objShader("../../MyWindow/rawShaders/objVertexShader.vs","../../MyWindow/rawShaders/blinnPhoneFregmentShader.fs");
    Shader borderRenderShader("../../MyWindow/rawShaders/borderRenderVertexShader.vs", "../../MyWindow/rawShaders/borderRenderFragmentShader.fs");
    Shader borderShader("../../MyWindow/rawShaders/borderVertexShader.vs", "../../MyWindow/rawShaders/borderFragmentShader.fs");
    Shader grassShader("../../MyWindow/rawShaders/grassVertexShader.vs", "../../MyWindow/rawShaders/grassFragmentShader.fs");
    Shader screenShader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/screenFragmentShader.fs");
    Shader skyboxShader("../../MyWindow/rawShaders/skyboxVertexShader.vs", "../../MyWindow/rawShaders/skyboxFragmentShader.fs");
    Shader mirrorCowShader("../../MyWindow/rawShaders/mirrorCowVertexShader.vs", "../../MyWindow/rawShaders/mirrorCowFragmentShader.fs");
    Shader shadowShader("../../MyWindow/rawShaders/shadowVertexShader.vs", "../../MyWindow/rawShaders/shadowFragmentShader.fs");
    Shader groundShader("../../MyWindow/rawShaders/groundVertexShader.vs", "../../MyWindow/rawShaders/groundFragmentShader.fs");
    Shader PBRShader("../../MyWindow/rawShaders/PBRVertexShader.vs", "../../MyWindow/rawShaders/PBRFragmentShader.fs");
    Shader FXAAShader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/FXAAFragmentShader.fs");

    float screenVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int screenVAO;
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

    float grassVertices[] = {
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 1.0f, 0.0f, 1.0f, 1.0f,
    };
    int width, height, nrChannels;
    unsigned char* data;

    unsigned int grassVAO;
    glGenVertexArrays(1, &grassVAO);
    unsigned int grassVBO;
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int grassTexture;
    glGenTextures(1, &grassTexture);
    glActiveTexture(GL_TEXTURE1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    data = stbi_load("../../MyWindow/image/grass.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    grassShader.use();
    grassShader.setInt("grassTex", 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    unsigned int fboColorTexture;
    glGenTextures(1, &fboColorTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTexture, 0);
    unsigned int fboDepthStencilTexture;
    glGenRenderbuffers(1, &fboDepthStencilTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencilTexture);
    glRenderbufferStorage(GL_RENDERBUFFER,  GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencilTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

 
    vector<std::string> faces
    {
        "../../MyWindow/skybox/px.png",
        "../../MyWindow/skybox/nx.png",
        "../../MyWindow/skybox/py.png",
        "../../MyWindow/skybox/ny.png",
        "../../MyWindow/skybox/pz.png",
        "../../MyWindow/skybox/nz.png",
    };
    glActiveTexture(GL_TEXTURE0);

    float planeVertices[] = {
        // positions                 // texcoords
         30.0f, -0.75f,  30.0f,   1.0f,  0.0f,
        -30.0f, -0.75f,  30.0f,     0.0f,  0.0f,
        -30.0f, -0.75f, -30.0f,     0.0f, 1.0f,
                          
         30.0f, -0.75f,  30.0f,  1.0f,  0.0f,
        -30.0f, -0.75f, -30.0f,     0.0f, 1.0f,
         30.0f, -0.75f, -30.0f,    1.0f, 1.0f

    };
    unsigned int planeVAO;
    glGenVertexArrays(1, &planeVAO);
    unsigned int planeVBO;
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int groundTex;
    glGenTextures(1, &groundTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTex);
    data = stbi_load("../../MyWindow/skybox/ny.png", &width, &height, &nrChannels, 0);
    //data = stbi_load("../../MyWindow/image/grass.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    
    //点光源设置


    //场景内的物体
    //todo:这里先临时直接加载，model是共享的，实际上运动物体有自己的model
    Model myModel("../../MyWindow/spot/spot_triangulated_good.obj");

    //todo:改成用工厂创建对象
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];

    PlasticMaterial* m1 = new PlasticMaterial();
    m1->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CommonObj* cow1 = new CommonObj(&myModel, m1);
    cow1->position = glm::vec3(-1.3, 0.0, -0.5);
    cow1->yaw = 180.0f;


    PBRMetalMaterial* m2 = new PBRMetalMaterial();
    m2->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CommonObj* cow2 = new CommonObj(&myModel, m2);
    cow2->yaw = 155.0f;

    PBRMetalMaterial* m3 = new PBRMetalMaterial();
    m3->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CommonObj* cow3 = new CommonObj(&myModel, m3);
    cow3->position = glm::vec3(1.1, 0.0, 0.5);
    cow3->yaw = 130.0f;

    content->allObjs->push_back(cow1);
    content->allObjs->push_back(cow2);
    content->allObjs->push_back(cow3);

    content->skyboxObj = new SkyboxObj();
    content->skyboxObj->material->SetTexture(TextureEnum::SkyboxTex_AutumnPark);
    content->skyboxObj->PrecomputeIBL();


    content->allLights->push_back(new Light(glm::vec3(6, 2.5, 4.5)));

    while (!glfwWindowShouldClose(window))
    {


        WindowCtrl::getInstance().Tick();

        glm::mat4 projection;
        glm::mat4 model;
        //-----------

        //渲染Pass
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        PBRShader.use();
        glBindVertexArray(screenVAO);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, content->allLights->at(0)->depthMapFBO->GetTexture("depthMap")->id);

        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gAlbedoSpec")->id);
         
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gFragColor")->id); 

        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gExtra")->id);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gPositionDepth")->id);
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gNormal")->id);
        glActiveTexture(GL_TEXTURE14);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gFragPosInLight")->id);

        PBRShader.setInt("shadowMap", 6);
        PBRShader.setInt("gExtra", 9);
        //PBRShader.setInt("gFragColor", 10);
        PBRShader.setInt("gPosition", 11);
        PBRShader.setInt("gNormal", 12);
        PBRShader.setInt("gAlbedo", 13);
        PBRShader.setInt("gFragPosLightSpace", 14);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().SSAOBlurFBO->GetTexture("ssaoBlurTex")->id);//todo:临时处理，这里是错误的
        PBRShader.setInt("texSSAO", 7);
        PBRShader.setMat4("view", content->mainCamera->GetViewMatrix());
        PBRShader.setVec3("lightPos", content->allLights->at(0)->lightPos);
        PBRShader.setVec3("lightColor", glm::vec3(50.0f, 50.0f, 50.0f));
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->IBLDiffuseLightTex->id);
        PBRShader.setInt("irradianceMap", 15);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->IBLSpecularLightTex->id);
        PBRShader.setInt("prefilterMap", 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, content->skyboxObj->material->IBLSpecularBRDFTex->id);
        PBRShader.setInt("brdfLUT", 5);
        PBRShader.setMat4("lightInverseProj", glm::inverse(content->allLights->at(0)->GetLightProjection()));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //将Gbuffer深度缓冲复制到fbo
        glBindFramebuffer(GL_READ_FRAMEBUFFER, RenderCtrl::getInstance().GBuffer->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); // 写入到默认帧缓冲
        glBlitFramebuffer(
            0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        groundShader.use();
        groundShader.setMat4("lightSpaceMatrix", content->allLights->at(0)->GetLightSpaceMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTex);
        groundShader.setInt("groundTex", 0);
        groundShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());
        groundShader.setMat4("view", content->mainCamera->GetViewMatrix());
        groundShader.setMat4("model", glm::mat4());
        groundShader.setVec3("lightPos", content->allLights->at(0)->lightPos);
        groundShader.setInt("shadowMap", 6);
        groundShader.setMat4("lightInverseProj", glm::inverse(content->allLights->at(0)->GetLightProjection()));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        glActiveTexture(GL_TEXTURE1);

        glBindTexture(GL_TEXTURE_CUBE_MAP, content->skyboxObj->material->cubeMapCache->id);

        //glBindTexture(GL_TEXTURE_CUBE_MAP, specularLightMap);
        skyboxShader.setInt("skybox", 1);

        skyboxShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(content->mainCamera->GetViewMatrix())));
        content->skyboxObj->Draw();
        glDepthFunc(GL_LESS);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        //为草排序，渲染草
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < 3; i++)
        {
            float distance = glm::length(content->mainCamera->Position - grassTranslate[i]);
            sorted[distance] = grassTranslate[i];
        }
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
            glm::mat4 projection;
            glm::mat4 model;
            grassShader.use();
            grassShader.setMat4("view", content->mainCamera->GetViewMatrix());
            grassShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());
            model = glm::mat4();
            model = glm::translate(model, it->second);
            glm::vec3 cameraDirec_xz = glm::normalize(glm::vec3(content->mainCamera->Direc.x, 0.0f, content->mainCamera->Direc.z));

            glm::vec3 n = glm::vec3(0, 0, -1);
            const glm::vec3 half = glm::normalize(cameraDirec_xz + n);
            const double w = glm::dot(n, half);
            const glm::vec3 xyz = glm::cross(n, half);
            const glm::quat p = glm::quat(w, xyz);

            glm::mat4 rotate = glm::mat4_cast(p);
            model = model * rotate;

            grassShader.setMat4("model", model);
            glBindVertexArray(grassVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        //----------渲染Pass 描边

        //复制模板缓冲
        glBindFramebuffer(GL_READ_FRAMEBUFFER, RenderCtrl::getInstance().GBuffer->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(
            0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_STENCIL_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glStencilFunc(GL_EQUAL, 0x0F, 0xFF);
        glStencilMask(0x00); // 禁止模板缓冲的写入

        borderRenderShader.use();
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(screenVAO);
        //borderRenderShader.setInt("colorTex", 15);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);
        //----------

        //将fbo绘制到输出缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (WindowCtrl::getInstance().window2Content[window]->useFXAA) {
            FXAAShader.use();
            glBindVertexArray(screenVAO);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, fboColorTexture);
            FXAAShader.setInt("screenTexture", 2);
        }
        else {
            screenShader.use();
            glBindVertexArray(screenVAO);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, fboColorTexture);

            screenShader.setInt("screenTexture", 2);
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);
        glfwPollEvents();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << err << std::endl;
        }
    }

    glfwTerminate();
    return 0;
}