#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Model.h"
#include <map> 
const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);
void drawObj(Shader objShader,Camera camera, Model myModel);
void drawFbo2Screen(Shader screenShader, Camera camera,unsigned int screenVAO, unsigned int fboColorTexture);
void drawBorder(Shader borderShader, Camera camera, Model myModel);
void drawGrass(Shader grassShader, Camera camera, unsigned int grassVAO);

glm::vec3 grassTranslate[] = {
    glm::vec3(0.5f,-0.5,2),glm::vec3(-1,-0.5,2),glm::vec3(0,-0.5,3)
};
Camera camera;
int main(void)

{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    stbi_set_flip_vertically_on_load(true);

    Shader lightShader("../../MyWindow/shaders/vertexShader.vs", "../../MyWindow/shaders/lightFregmentShader.fs");
    Shader objShader("../../MyWindow/shaders/vertexShader.vs", "../../MyWindow/shaders/objFregmentShader.fs");
    Shader borderShader("../../MyWindow/shaders/borderVertexShader.vs", "../../MyWindow/shaders/borderFragmentShader.fs");
    Shader grassShader("../../MyWindow/shaders/grassVertexShader.vs", "../../MyWindow/shaders/grassFragmentShader.fs");
    Shader screenShader("../../MyWindow/shaders/screenVertexShader.vs", "../../MyWindow/shaders/screenFragmentShader.fs");
    Shader skyboxShader("../../MyWindow/shaders/skyboxVertexShader.vs", "../../MyWindow/shaders/skyboxFragmentShader.fs");
    Shader mirrorCowShader("../../MyWindow/shaders/mirrorCowVertexShader.vs", "../../MyWindow/shaders/mirrorCowFragmentShader.fs");
    Model myModel("../../MyWindow/spot/spot_triangulated_good.obj");

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
    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    data = stbi_load("../../MyWindow/spot/spot_texture.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    objShader.use();
    objShader.setInt("spotTex", 0);
    grassShader.use();
    grassShader.setInt("grassTex", 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    unsigned int fboColorTexture;
    glGenTextures(1, &fboColorTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fboColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTexture, 0);
    unsigned int fboDepthStencilTexture;
    glGenRenderbuffers(1, &fboDepthStencilTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencilTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
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
    glActiveTexture(GL_TEXTURE3);
    unsigned int skyboxTexture = loadCubemap(faces);

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
    ////创建新的动态贴图
    unsigned int dynamicBoxTex;
    glGenTextures(1, &dynamicBoxTex);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicBoxTex);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    ////生成六个fbo
    unsigned int fboArray[6];
    glGenFramebuffers(6, fboArray);
    for (int i = 0; i < 6; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fboArray[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dynamicBoxTex, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glm::mat4 projection;
        glm::mat4 model;

        for (int i = 0; i < 6; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, fboArray[i]);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, 1024, 1024);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            Camera tempCamera = Camera();
            float Yaw = 0;
            float Pitch = 0;
            if (i == 0) {
                Yaw = 90.0f;
            }
            else if (i == 1) {
                Yaw = 270.0f;
            }
            else if (i == 2) {
                Pitch = -90.0f;
            }
            else if (i == 3) {
                Pitch = 90.0f;
            }
            else if (i == 4) {

            }
            else if (i == 5) {
                Yaw = 180.0f;
            }
            tempCamera.Yaw = Yaw;
            tempCamera.Pitch = Pitch;
            tempCamera.ProcessMouseMovement(0, 0);
            tempCamera.Position = glm::vec3(-1.0, 0.0, -1.0);
            //std::cout << i << " " << tempCamera.Direc.x << " " << tempCamera.Direc.y << " " << tempCamera.Direc.z << std::endl;
            //std::cout << tempCamera.Up.x << " " << tempCamera.Up.y << " " << tempCamera.Up.z << std::endl;
            //std::cout << tempCamera.Right.x << " " << tempCamera.Right.y << " " << tempCamera.Right.z << std::endl;

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
            glStencilMask(0xFF); // 启用模板缓冲写入
            glm::vec3 pointLightPos1(-1, 3, 3);
            glm::vec3 pointLightPos2(3, 3, 1);
            objShader.use();
            objShader.setMat4("view", tempCamera.GetViewMatrix());
            projection = glm::mat4();
            projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
            objShader.setMat4("projection", projection);
            model = glm::mat4();
            model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
            objShader.setMat4("model", model);
            objShader.setVec3("lightPos[0]", pointLightPos1);
            objShader.setVec3("lightPos[1]", pointLightPos2);
            myModel.Draw(objShader);

            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            skyboxShader.setInt("skybox", 3);
            projection = glm::mat4();
            projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
            skyboxShader.setMat4("projection", projection);
            skyboxShader.setMat4("view", glm::mat4(glm::mat3(tempCamera.GetViewMatrix())));
            glBindVertexArray(skyboxVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);

            std::map<float, glm::vec3> sorted;
            for (unsigned int i = 0; i < 3; i++)
            {
                float distance = glm::length(tempCamera.Position - grassTranslate[i]);
                sorted[distance] = grassTranslate[i];
            }
            for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
                grassShader.use();
                grassShader.setMat4("view", tempCamera.GetViewMatrix());
                projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
                grassShader.setMat4("projection", projection);
                model = glm::mat4();
                model = glm::translate(model, it->second);
                glm::vec3 cameraDirec_xz = glm::normalize(glm::vec3(tempCamera.Direc.x, 0.0f, tempCamera.Direc.z));

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
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        drawObj(objShader, camera, myModel);

        mirrorCowShader.use();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
        model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
        projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
        mirrorCowShader.setMat4("view", camera.GetViewMatrix());
        mirrorCowShader.setVec3("cameraPos", camera.Position);
        mirrorCowShader.setMat4("projection", projection);
        mirrorCowShader.setMat4("model", model);
        mirrorCowShader.setInt("skybox",4);
        myModel.Draw(mirrorCowShader);

        //mirrorCowShader.use();
        //glBindVertexArray(cubeVAO);
        //model = glm::mat4();
        //model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
        //projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
        //mirrorCowShader.setMat4("view", camera.GetViewMatrix());
        //mirrorCowShader.setVec3("cameraPos", camera.Position);
        //mirrorCowShader.setMat4("projection", projection);
        //mirrorCowShader.setMat4("model", model);
        //mirrorCowShader.setInt("skybox", 4);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilMask(0x00);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setInt("skybox", 3);
        projection = glm::mat4();
        projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        drawGrass(grassShader, camera, grassVAO);
        drawBorder(borderShader, camera, myModel);
        drawFbo2Screen(screenShader, camera, screenVAO, fboColorTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(screenVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fboColorTexture);
        screenShader.setInt("screenTexture", 2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
void processInput(GLFWwindow* window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(DirecEnum::Front, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DirecEnum::Back, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(DirecEnum::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(DirecEnum::Right,deltaTime);
}

float lastX = screenWidth/2, lastY = screenHeight/2;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    camera.ProcessMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos;
    lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    stbi_set_flip_vertically_on_load(false);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void drawObj(Shader objShader, Camera camera, Model myModel) {
    glm::mat4 projection;
    glm::mat4 model;
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
    glStencilMask(0xFF); // 启用模板缓冲写入
    glm::vec3 pointLightPos1(-1, 3, 3);
    glm::vec3 pointLightPos2(3, 3, 1);
    objShader.use();
    objShader.setMat4("view", camera.GetViewMatrix());
    projection = glm::mat4();
    projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
    objShader.setMat4("projection", projection);
    model = glm::mat4();
    model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
    objShader.setMat4("model", model);
    objShader.setVec3("lightPos[0]", pointLightPos1);
    objShader.setVec3("lightPos[1]", pointLightPos2);
    myModel.Draw(objShader);
}

void drawFbo2Screen(Shader screenShader, Camera camera, unsigned int screenVAO, unsigned int fboColorTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    screenShader.use();
    glBindVertexArray(screenVAO);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fboColorTexture);
    screenShader.setInt("screenTexture", 2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawBorder(Shader borderShader, Camera camera, Model myModel) {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // 禁止模板缓冲的写入
    glDisable(GL_DEPTH_TEST);
    borderShader.use();

    glm::mat4 projection;
    glm::mat4 model;
    borderShader.setMat4("view", camera.GetViewMatrix());
    projection = glm::mat4();
    projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
    borderShader.setMat4("projection", projection);
    model = glm::mat4();
    model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
    borderShader.setMat4("model", model);
    myModel.Draw(borderShader);

    model = glm::mat4();
    model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
    model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
    borderShader.setMat4("model", model);
    myModel.Draw(borderShader);

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

void drawGrass(Shader grassShader, Camera camera, unsigned int grassVAO) {
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < 3; i++)
    {
        float distance = glm::length(camera.Position - grassTranslate[i]);
        sorted[distance] = grassTranslate[i];
    }
    for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
        glm::mat4 projection;
        glm::mat4 model;
        grassShader.use();
        grassShader.setMat4("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
        grassShader.setMat4("projection", projection);
        model = glm::mat4();
        model = glm::translate(model, it->second);
        glm::vec3 cameraDirec_xz = glm::normalize(glm::vec3(camera.Direc.x, 0.0f, camera.Direc.z));

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
}