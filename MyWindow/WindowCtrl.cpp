#include "WindowCtrl.h"
#include "InputCtrl.h"

WindowCtrl& WindowCtrl::getInstance() {
	static WindowCtrl instance;
	return instance;
}

void WindowCtrl::Tick() {
	float curFrameTime = glfwGetTime();
	for (const auto& pair : window2Content) {
		GLFWwindow* window = pair.first;
		WindowContent* content = pair.second;
		//����ʱ��
		content->lastFrameTime = content->curFrameTime;
		content->curFrameTime = curFrameTime;
		content->frameDeltaTime = content->curFrameTime = content->lastFrameTime;
		//�������
		InputCtrl::getInstance().processHotKeyInput(window);
	}
}

WindowCtrl::WindowCtrl() {}
WindowCtrl::~WindowCtrl() {}