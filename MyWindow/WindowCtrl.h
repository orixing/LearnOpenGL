#pragma once
#include <unordered_map>
#include <GLFW/glfw3.h>
#include "Camera.h"

//一个Window的上下文数据
class WindowContent {
public:
	Camera* mainCamera;
	bool useFXAA;
	float mousePosX;
	float mousePosY;
	bool firstMouse;

	float curFrameTime;
	float lastFrameTime;
	float frameDeltaTime;
};

class WindowCtrl
{
public:
	static WindowCtrl& getInstance();

	WindowCtrl(const WindowCtrl& windowCtrl) = delete;
	WindowCtrl& operator=(const WindowCtrl& windowCtrl) = delete;

	std::unordered_map<GLFWwindow*, WindowContent*> window2Content;

	void Tick();

private:
	WindowCtrl();
	~WindowCtrl();
};