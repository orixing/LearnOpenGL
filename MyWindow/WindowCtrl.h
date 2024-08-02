#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "Camera.h"
#include "WindowContent.h"

class WindowCtrl
{
public:
	static WindowCtrl& getInstance();

	WindowCtrl(const WindowCtrl& windowCtrl) = delete;
	WindowCtrl& operator=(const WindowCtrl& windowCtrl) = delete;

	std::unordered_map<GLFWwindow*, WindowContent*> window2Content;

	void Tick();

	static void windowSizecallback(GLFWwindow* window, int width, int height);
	static void processHotKeyInput(GLFWwindow* window);
	static void mouseInputCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollInputCallback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* NewWindow();

	static const int ScreenWidth = 800; 
	static const int ScreenHeight = 600;

private:
	WindowCtrl();
	~WindowCtrl();
};