#include "Camera.h"
#include "CommonObj.h"
#include "GameObj.h"
#include "BillboardingObj.h"
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
#include "StaticEntity.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random> 

int main(void)
{
    //glEnable(GL_MULTISAMPLE);
    //glfwWindowHint(GLFW_SAMPLES, 4);//todo:这个msaa选项只影响下一个创建的窗口 后置渲染 不用管了
    GLFWwindow* window = WindowCtrl::getInstance().NewWindow();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //透明物体渲染的设置
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stbi_set_flip_vertically_on_load(true);
  
    //todo:可能改成用工厂创建对象
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];

    //添加地板
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
    groundMaterial->metallic = 0.0f;
    groundMaterial->roughness = 1.0f;
    CommonObj* ground = new CommonObj(groundModel, groundMaterial);
    StaticEntity* physical = new StaticEntity(ground);
    ground->physical = physical;
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

    //天空盒

    content->skyboxObj = new SkyboxObj();
    content->skyboxObj->material->SetTexture(TextureEnum::SkyboxTex_AutumnPark);
    content->skyboxObj->PrecomputeIBL();

    //多光源
    content->allLights->push_back(new Light(glm::vec3(0, 40, 45)));
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
    grassMaterial->shader = RenderCtrl::getInstance().grassShader;
    BillboardingObj* grass1 = new BillboardingObj(grassModel, grassMaterial);
    grass1->transparent = true;
    grass1->position = glm::vec3(0.5f, -0.5, 2);
    content->allObjs->push_back(grass1);

    BillboardingObj* grass2 = new BillboardingObj(grassModel, grassMaterial);
    grass2->transparent = true;
    grass2->position = glm::vec3(-1, -0.5, 2);
    content->allObjs->push_back(grass2);

    BillboardingObj* grass3 = new BillboardingObj(grassModel, grassMaterial);
    grass3->transparent = true;
    grass3->position = glm::vec3(0, -0.5, 3);
    content->allObjs->push_back(grass3);

    while (!glfwWindowShouldClose(window))
    {
        WindowCtrl::getInstance().Tick();
    }

    glfwTerminate();
    return 0;
}