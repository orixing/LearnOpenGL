#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Model.h"
#include <map> 
#include <random> 
#include "GameObj.h"
#include "CowObj.h"
#include "PlasticMaterial.h"
#include "PBRMetalMaterial.h"
#include "RenderCtrl.h"
#include "TextureCtrl.h"
#include "WindowCtrl.h"
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
    Shader SSAOShader("../../MyWindow/rawShaders/SSAOVertexShader.vs", "../../MyWindow/rawShaders/SSAOFregmentShader.fs");
    Shader SSAOBlurShader("../../MyWindow/rawShaders/SSAOVertexShader.vs", "../../MyWindow/rawShaders/SSAOBlurFragmentShader.fs");
    Shader PBRShader("../../MyWindow/rawShaders/PBRVertexShader.vs", "../../MyWindow/rawShaders/PBRFragmentShader.fs");
    Shader HDRCubeShader("../../MyWindow/rawShaders/HDR2CubeVertexShader.vs", "../../MyWindow/rawShaders/HDR2CubeFragmentShader.fs");
    Shader diffuseCubeShader("../../MyWindow/rawShaders/diffuseCubeVertexShader.vs", "../../MyWindow/rawShaders/diffuseCubeFragmentShader.fs");
    Shader specularLightShader("../../MyWindow/rawShaders/diffuseCubeVertexShader.vs", "../../MyWindow/rawShaders/specularCubeFragmentShader.fs");
    Shader specularBRDFShader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/specularBRDFFragmentShader.fs");
    Shader FXAAShader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/FXAAFragmentShader.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, 
    };
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    unsigned int lightVBO;
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    //objShader.use();
    //objShader.setInt("spotTex", 0);
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
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fboColorTexture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_SRGB, screenWidth, screenHeight, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fboColorTexture, 0);
    unsigned int fboDepthStencilTexture;
    glGenRenderbuffers(1, &fboDepthStencilTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencilTexture);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencilTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
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

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    unsigned int skyboxVBO;
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

    //---------------预准备SSAO数据
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // 随机浮点数，范围0.0 - 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
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
        ssaoKernel.push_back(sample);
        ssaoKernel.push_back(sample);
    }

    std::vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    GLuint ssaoNoiseTexture;
    glGenTextures(1, &ssaoNoiseTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    GLuint ssaoColorBuffer;

    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    GLuint ssaoBlurFBO, ssaoColorBufferBlur;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    //---------------

    stbi_set_flip_vertically_on_load(true);
    int  nrComponents;
    float* fData = stbi_loadf("../../MyWindow/skybox/autumn_park_16k.hdr", &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (fData)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, fData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(fData);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }


    float cubeVertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
         // bottom face
         -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
          1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
          1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
          1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
         -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         // top face
         -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
          1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
          1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
          1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    };
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int cubeFBO, cubeRBO;
    glGenFramebuffers(1, &cubeFBO);
    glGenRenderbuffers(1, &cubeRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 4096, 4096);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubeRBO);

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            4096, 4096, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    //-----------准备HDR天空盒
    HDRCubeShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    HDRCubeShader.setInt("equirectangularMap", 0);
    HDRCubeShader.setMat4("projection", captureProjection);
    glViewport(0, 0, 4096, 4096); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        HDRCubeShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //-----------
    //-----------准备diffuse天空盒
    unsigned int diffuseMap;
    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,
            GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);

    diffuseCubeShader.use();
    diffuseCubeShader.setInt("environmentMap", 0);
    diffuseCubeShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 128, 128); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        diffuseCubeShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //-----------
    //------------准备specular天空盒
    unsigned int specularLightMap;
    glGenTextures(1, &specularLightMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specularLightMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    specularLightShader.use();
    specularLightShader.setInt("environmentMap", 0);
    specularLightShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 1024 * std::pow(0.5, mip);
        unsigned int mipHeight = 1024 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        specularLightShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            specularLightShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularLightMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //-----------
    //------------准备specular BRDF
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    specularBRDFShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //点光源设置
    glm::vec3 pointLightPos1(6, 2.5, 4.5);
    glm::mat4 lightView = glm::lookAt(pointLightPos1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    //场景内的物体
    //todo:这里先临时直接加载，共享型model需要加载器
    Model myModel("../../MyWindow/spot/spot_triangulated_good.obj");

    //todo:改成用工厂创建对象
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];

    CowObj* cow1 = new CowObj(&myModel, new PlasticMaterial());
    cow1->position = glm::vec3(-1.3, 0.0, -0.5);
    cow1->yaw = 180.0f;
    cow1->material->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CowObj* cow2 = new CowObj(&myModel, new PBRMetalMaterial());
    cow2->yaw = 155.0f;
    cow2->material->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CowObj* cow3 = new CowObj(&myModel, new PBRMetalMaterial());
    cow3->position = glm::vec3(1.1, 0.0, 0.5);
    cow3->yaw = 130.0f;
    cow3->material->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    content->allObjs->push_back(cow1);
    content->allObjs->push_back(cow2);
    content->allObjs->push_back(cow3);

    while (!glfwWindowShouldClose(window))
    {


        WindowCtrl::getInstance().Tick();

        glm::mat4 projection;
        glm::mat4 model;

        //----------------SSAO
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glBindVertexArray(screenVAO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT);
        SSAOShader.use();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gPositionDepth")->id);//todo:思考gPositionDepth等一系列GBuffer存放在哪合适
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, RenderCtrl::getInstance().GBuffer->GetTexture("gNormal")->id);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
        for (GLuint i = 0; i < 64; ++i) {
            SSAOShader.setVec3(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
        }
        SSAOShader.setInt("gPositionDepth", 3);
        SSAOShader.setInt("gNormal", 4);
        SSAOShader.setInt("texNoise", 5);
        SSAOShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        SSAOBlurShader.use();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        SSAOBlurShader.setInt("ssaoInput",3);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        //---------------

        //-----------Shadow Pass
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        shadowShader.use();
        shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        model = glm::mat4();
        model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
        shadowShader.setMat4("model", model);
        myModel.Draw();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.3, 0.0, -0.5));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        shadowShader.setMat4("model", model);
        myModel.Draw();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(1.1, 0.0, 0.5));
        model = glm::rotate(model, glm::radians(130.0f), glm::vec3(0.0, 1.0, 0.0));
        shadowShader.setMat4("model", model);
        myModel.Draw();
        //-----------

        //渲染Pass
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        PBRShader.use();
        glBindVertexArray(screenVAO);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, depthMap);

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
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        PBRShader.setInt("texSSAO", 7);
        PBRShader.setMat4("view", content->mainCamera->GetViewMatrix());
        PBRShader.setVec3("lightPos", pointLightPos1);
        PBRShader.setVec3("lightColor", glm::vec3(50.0f, 50.0f, 50.0f));
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseMap);
        PBRShader.setInt("irradianceMap", 15);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, specularLightMap);
        PBRShader.setInt("prefilterMap", 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        PBRShader.setInt("brdfLUT", 5);
        PBRShader.setMat4("lightInverseProj", glm::inverse(lightProjection));
        glDrawArrays(GL_TRIANGLES, 0, 6);



        glViewport(0, 0, screenWidth, screenHeight);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, RenderCtrl::getInstance().GBuffer->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); // 写入到默认帧缓冲
        glBlitFramebuffer(
            0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        groundShader.use();
        groundShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTex);
        groundShader.setInt("groundTex", 0);
        groundShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());
        groundShader.setMat4("view", content->mainCamera->GetViewMatrix());
        groundShader.setMat4("model", glm::mat4());
        groundShader.setVec3("lightPos", pointLightPos1);
        groundShader.setInt("shadowMap", 6);
        groundShader.setMat4("lightInverseProj", glm::inverse(lightProjection));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, hdrTexture); 
        //glBindTexture(GL_TEXTURE_CUBE_MAP, specularLightMap);
        skyboxShader.setInt("skybox", 0);
        skyboxShader.setMat4("projection", content->mainCamera->GetProjectionMatrix());
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(content->mainCamera->GetViewMatrix())));
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

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


        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
    }

    glfwTerminate();
    return 0;
}