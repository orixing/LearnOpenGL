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

const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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

    Shader lightShader("shaders/vertexShader.vs", "shaders/lightFregmentShader.fs");
    Shader objShader("shaders/vertexShader.vs", "shaders/objFregmentShader.fs");
    Shader borderShader("shaders/borderVertexShader.vs", "shaders/borderFragmentShader.fs");
    Model myModel("spot/spot_triangulated_good.obj");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, 

         //0.0f,  0.0f, 0.5f,
         //-0.1f,  0.0f, 0.0f,
         //0.0f,  0.1f, 0.0f,

         //-100.0f,-20.0f,-100.0f,
         //-100.0f,-20.0f,100.0f,
         //100.0f,-20.0f,100.0f,

         //100.0f,-20.0f,-100.0f,
         //100.0f,-20.0f,100.0f,
         //-100.0f,-20.0f,-100.0f,
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

    int width, height, nrChannels;
    unsigned char* data;
    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    data = stbi_load("spot/spot_texture.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    objShader.use();
    objShader.setInt("spotTex", 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //glm::vec3 pointLightPos(sin(glfwGetTime())*5,5+ sin(glfwGetTime()) * 5, sin(glfwGetTime()) *5);
        //glm::vec3 pointLightPos(sin(glfwGetTime()) * 3, cos(glfwGetTime()) * 3, sin(glfwGetTime()) * -3);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
        glStencilMask(0xFF); // 启用模板缓冲写入
        glm::vec3 pointLightPos1(-1, 3, 3);
        glm::vec3 pointLightPos2(3, 3, 1);
        objShader.use();
        objShader.setMat4("view", camera.GetViewMatrix());
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
        objShader.setMat4("projection", projection);
        glm::mat4 model;
        model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0,1.0,0.0));
        objShader.setMat4("model", model);
        objShader.setVec3("lightPos[0]", pointLightPos1);
        objShader.setVec3("lightPos[1]", pointLightPos2);
        myModel.Draw(objShader);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
        model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
        objShader.setMat4("model", model);
        myModel.Draw(objShader);


        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // 禁止模板缓冲的写入
        glDisable(GL_DEPTH_TEST);
        borderShader.use();

        borderShader.setMat4("view", camera.GetViewMatrix());
        projection = glm::mat4();
        projection = glm::perspective(glm::radians(camera.fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
        borderShader.setMat4("projection", projection);
        model = glm::mat4();
        model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
        borderShader.setMat4("model", model);
        borderShader.setVec3("lightPos[0]", pointLightPos1);
        borderShader.setVec3("lightPos[1]", pointLightPos2);
        myModel.Draw(borderShader);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
        model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0, 1.0, 0.0));
        borderShader.setMat4("model", model);
        myModel.Draw(borderShader);

        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);

        //lightShader.use();
        //lightShader.setMat4("view", camera.GetViewMatrix());
        //lightShader.setMat4("projection", projection);
        //model = glm::mat4();
        //model = glm::translate(model, pointLightPos2);
        //model = glm::scale(model, glm::vec3(0.2f));
        //lightShader.setMat4("model", model);
        //glBindVertexArray(lightVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

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