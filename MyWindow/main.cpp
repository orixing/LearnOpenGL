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
#include "TransparentMaterial.h"
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

    Shader grassShader("../../MyWindow/rawShaders/grassVertexShader.vs", "../../MyWindow/rawShaders/grassFragmentShader.fs");
    Shader screenShader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/screenFragmentShader.fs");
    Shader shadowShader("../../MyWindow/rawShaders/shadowVertexShader.vs", "../../MyWindow/rawShaders/shadowFragmentShader.fs");
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

    //透明物体渲染的设置
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
    //todo:可能改成用工厂创建对象
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];

    //添加地板
    //构造mesh
    vector<float> planeVertices = {
    30.0f, -0.75f, 30.0f, 0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
    -30.0f, -0.75f, 30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -30.0f, -0.75f, -30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
    30.0f, -0.75f, -30.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f
    };
    vector<unsigned int> groundIndices = { 0,1,2, 0,2,3 };
    Mesh* groundMesh = new Mesh(Vertex::genVertexByRawData(planeVertices), groundIndices);
    Model* groundModel = new Model(groundMesh);
    PBRMetalMaterial* groundMaterial = new PBRMetalMaterial();
    groundMaterial->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::GroundTex);
    groundMaterial->metallic = 1.0f;
    groundMaterial->roughness = 1.0f;
    CommonObj* ground = new CommonObj(groundModel, groundMaterial);
    content->allObjs->push_back(ground);


    //场景内的物体
    //todo:这里先临时直接加载，model是共享的，实际上运动物体有自己的model
    Model myModel("../../MyWindow/spot/spot_triangulated_good.obj");

    PlasticMaterial* m1 = new PlasticMaterial();
    m1->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CommonObj* cow1 = new CommonObj(&myModel, m1);
    cow1->position = glm::vec3(-1.3, 0.0, -0.5);
    cow1->yaw = 180.0f;
    cow1->renderBorder = true;


    PBRMetalMaterial* m2 = new PBRMetalMaterial();
    m2->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    m2->metallic = 0.8f;
    m2->roughness = 0.1f;
    CommonObj* cow2 = new CommonObj(&myModel, m2);
    cow2->yaw = 155.0f;
    cow2->renderBorder = true;

    PBRMetalMaterial* m3 = new PBRMetalMaterial();
    m3->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    m3->metallic = 0.2f;
    m3->roughness = 0.8f;
    CommonObj* cow3 = new CommonObj(&myModel, m3);
    cow3->position = glm::vec3(1.1, 0.0, 0.5);
    cow3->yaw = 130.0f;
    cow3->renderBorder = true;

    content->allObjs->push_back(cow1);
    content->allObjs->push_back(cow2);
    content->allObjs->push_back(cow3);

    content->skyboxObj = new SkyboxObj();
    content->skyboxObj->material->SetTexture(TextureEnum::SkyboxTex_AutumnPark);
    content->skyboxObj->PrecomputeIBL();

    //多光源
    content->allLights->push_back(new Light(glm::vec3(6, 2.5, 4.5)));
    //content->allLights->push_back(new Light(glm::vec3(4.5, 2.5, 6)));


    //透明草，静态草直接共用一个model和材质就行
    vector<float> grassVertices = {
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    };
    vector<unsigned int> grassIndices = { 0,1,2,2,1,3 };

    Mesh* grassMesh = new Mesh(Vertex::genVertexByRawData(grassVertices), grassIndices);
    Model* grassModel = new Model(grassMesh);
    TransparentMaterial* grassMaterial = new TransparentMaterial();
    grassMaterial->texture = TextureCtrl::getInstance().getTexture(TextureEnum::GrassTex);
    CommonObj* grass1 = new CommonObj(grassModel, grassMaterial);
    grass1->transparent = true;
    grass1->position = glm::vec3(0.5f, -0.5, 2);
    content->allObjs->push_back(grass1);

    CommonObj* grass2 = new CommonObj(grassModel, grassMaterial);
    grass2->transparent = true;
    grass2->position = glm::vec3(-1, -0.5, 2);
    content->allObjs->push_back(grass2);

    CommonObj* grass3 = new CommonObj(grassModel, grassMaterial);
    grass3->transparent = true;
    grass3->position = glm::vec3(0, -0.5, 3);
    content->allObjs->push_back(grass3);

    while (!glfwWindowShouldClose(window))
    {


        WindowCtrl::getInstance().Tick();

        grassShader.use();
        grassShader.setInt("grassTex", 0);
        grassShader.setMat4("view", content->mainCamera->GetViewMatrix());
        grassShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());

        //为透明物体排序，渲染草
        std::map<float, GameObj*> sorted;
        for (int i = 0; i < content->allObjs->size(); i++) {
            GameObj* obj = content->allObjs->at(i);
            if (!obj->transparent) continue;
            float distance = glm::length(content->mainCamera->Position - obj->position);
            sorted[distance] = obj;
        }
        for (std::map<float, GameObj*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
            GameObj* obj = it->second;
            TransparentMaterial* material = dynamic_cast<TransparentMaterial*>(dynamic_cast<CommonObj*>(obj)->material);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material -> texture -> id);

            //草转向相机
            glm::mat4 model = glm::mat4();
            model = glm::translate(model, it->second->position);
            glm::vec3 cameraDirec_xz = glm::normalize(glm::vec3(content->mainCamera->Direc.x, 0.0f, content->mainCamera->Direc.z));
            glm::vec3 n = glm::vec3(0, 0, -1);
            const glm::vec3 half = glm::normalize(cameraDirec_xz + n);
            const double w = glm::dot(n, half);
            const glm::vec3 xyz = glm::cross(n, half);
            const glm::quat p = glm::quat(w, xyz);

            glm::mat4 rotate = glm::mat4_cast(p);
            model = model * rotate;

            grassShader.setMat4("model", model);
            obj->Draw();
        }


        //----------渲染Pass 描边

        //复制模板缓冲
        glBindFramebuffer(GL_READ_FRAMEBUFFER, RenderCtrl::getInstance().GBuffer->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderCtrl::getInstance().postProcessingFBO->id);
        glBlitFramebuffer(
            0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_STENCIL_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, RenderCtrl::getInstance().postProcessingFBO->id);

        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_NOTEQUAL, 0xFF, 0xFF);
        glStencilMask(0x0F);

        Shader* bShader = new Shader("../../MyWindow/rawShaders/GBorderVertexShader.vs", "../../MyWindow/rawShaders/borderRenderFragmentShader.fs");

        bShader->use();
        bShader->setMat4("view", content->mainCamera->GetViewMatrix());
        bShader->setMat4("projection", content->mainCamera->GetProjectionMatrix());

        for (GameObj* oneObj : *content->allObjs) {
            if(oneObj->renderBorder)
                oneObj->DrawInGPass(bShader);
        }

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
            glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().postProcessingFBO->GetTexture("screenTex")->id);
            FXAAShader.setInt("screenTexture", 2);
        }
        else {
            screenShader.use();
            glBindVertexArray(screenVAO);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().postProcessingFBO->GetTexture("screenTex")->id);

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