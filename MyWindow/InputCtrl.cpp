#include "InputCtrl.h"
#include "CameraCtrl.h"
#include "WindowCtrl.h"
InputCtrl& InputCtrl::getInstance() {
	static InputCtrl instance;
	return instance;
}


InputCtrl::InputCtrl() {}
InputCtrl::~InputCtrl() {}

void InputCtrl::windowSizecallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void InputCtrl::processHotKeyInput(GLFWwindow* window)
{
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        CameraCtrl::getInstance().mainCamera->ProcessKeyboard(DirecEnum::Front, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        CameraCtrl::getInstance().mainCamera->ProcessKeyboard(DirecEnum::Back, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        CameraCtrl::getInstance().mainCamera->ProcessKeyboard(DirecEnum::Left, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        CameraCtrl::getInstance().mainCamera->ProcessKeyboard(DirecEnum::Right, content->frameDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        !content->useFXAA;
}

void InputCtrl::mouseInputCallback(GLFWwindow* window, double xpos, double ypos) {
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

void InputCtrl::scrollInputCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    WindowContent* content = WindowCtrl::getInstance().window2Content[window];
    content->mainCamera->ProcessMouseScroll(yoffset);
}
