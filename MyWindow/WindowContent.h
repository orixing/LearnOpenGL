#pragma once
#include "Camera.h"
#include <vector>
#include "GameObj.h"
#include "Light.h"
#include "SkyboxObj.h"
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

	std::vector<GameObj*> *allObjs;
	std::vector<Light*>* allLights;

	SkyboxObj* skyboxObj;

	FrameBuffer* GBuffer, * SSAOFBO, * SSAOBlurFBO, * postProcessingFBO;

	WindowContent(Camera* c);
	~WindowContent();

	void ShootNewObj();//todo:使用工厂实现，可能需要换个地方

	Model* model;//todo:一个model只加载一次，创建Obj的工厂

};