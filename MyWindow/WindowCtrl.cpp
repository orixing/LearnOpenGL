#include "WindowCtrl.h"
#include "WindowContent.h"
#include <iostream>
#include "RenderCtrl.h"
WindowCtrl& WindowCtrl::getInstance() {
	static WindowCtrl instance;
	return instance;
}

GLFWwindow* WindowCtrl::NewWindow() {
    GLFWwindow* window = glfwCreateWindow(GlobalConst::ScreenWidth, GlobalConst::ScreenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, WindowCtrl::windowSizecallback);
    glfwSetCursorPosCallback(window, WindowCtrl::mouseInputCallback);
    glfwSetScrollCallback(window, WindowCtrl::scrollInputCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    WindowCtrl::getInstance().window2Content[window] = new WindowContent(new Camera(glm::vec3(0.0f, 0.0f, 4.0f)));

    return window;
}

void WindowCtrl::Tick() {
	float curFrameTime = glfwGetTime();
	for (const auto& pair : window2Content) {
		GLFWwindow* window = pair.first;
		WindowContent* content = pair.second;
		//更新时间
		content->lastFrameTime = content->curFrameTime;
		content->curFrameTime = curFrameTime;
		content->frameDeltaTime = content->curFrameTime - content->lastFrameTime;
		//检测输入
        processHotKeyInput(window);
        //物理模拟
        for (GameObj* obj : *content->allObjs) {
            if (obj->physical == NULL) continue;
            obj->physical->BeforeCollision();
            //这里先只做地面和物体之间的碰撞检测
            obj->physical->HandleCollision(nullptr);
            obj->physical->AfterCollision();
        }
        //渲染窗口
        RenderCtrl::getInstance().Render(content);

        glfwSwapBuffers(window);
        glfwPollEvents();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << err << std::endl;
        }
	}
}

WindowCtrl::WindowCtrl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
WindowCtrl::~WindowCtrl() {}

void WindowCtrl::windowSizecallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void WindowCtrl::processHotKeyInput(GLFWwindow* window)
{
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        content->mainCamera->ProcessKeyboard(DirecEnum::Front, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        content->mainCamera->ProcessKeyboard(DirecEnum::Back, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        content->mainCamera->ProcessKeyboard(DirecEnum::Left, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        content->mainCamera->ProcessKeyboard(DirecEnum::Right, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        content->useFXAA = !content->useFXAA;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        content->ShootNewObj();
}

void WindowCtrl::mouseInputCallback(GLFWwindow* window, double xpos, double ypos) {
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];
    if (content->firstMouse)
    {
        content->mousePosX = xpos;
        content->mousePosY = ypos;
        content->firstMouse = false;
    }
    content->mainCamera->ProcessMouseMovement(xpos - content->mousePosX, content->mousePosY - ypos);
    content->mousePosX = xpos;
    content->mousePosY = ypos;
}

void WindowCtrl::scrollInputCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];
    content->mainCamera->ProcessMouseScroll(yoffset);
}
