#pragma once
#include <GLFW/glfw3.h>
#include "WindowCtrl.h"
class InputCtrl
{
public:
	static InputCtrl& getInstance();

	InputCtrl(const InputCtrl& inputCtrl) = delete;
	InputCtrl& operator=(const InputCtrl& inputCtrl) = delete;

	static void windowSizecallback(GLFWwindow* window, int width, int height);
	void processHotKeyInput(GLFWwindow* window);
	static void mouseInputCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollInputCallback(GLFWwindow* window, double xoffset, double yoffset);
private:
	InputCtrl();
	~InputCtrl();
};

