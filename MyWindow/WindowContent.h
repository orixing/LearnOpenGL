#pragma once
#include "Camera.h"
#include <vector>
#include "GameObj.h"
#include "Light.h"
//һ��Window������������
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

	std::vector<GameObj*> *allObjs;
	std::vector<Light*>* allLights;

	WindowContent(Camera* c);
	~WindowContent();
};